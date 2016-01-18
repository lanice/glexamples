#include "AtomicCounter.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/boolean.h>

#include <globjects/globjects.h>
#include <globjects/logging.h>
#include <globjects/DebugMessage.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/Framebuffer.h>
#include <globjects/Buffer.h>

#include <gloperate/base/RenderTargetType.h>

#include <gloperate/painter/TargetFramebufferCapability.h>
#include <gloperate/painter/ViewportCapability.h>
#include <gloperate/painter/PerspectiveProjectionCapability.h>
#include <gloperate/painter/CameraCapability.h>
#include <gloperate/painter/TypedRenderTargetCapability.h>
#include <gloperate/painter/VirtualTimeCapability.h>

#include <gloperate/primitives/Icosahedron.h>
#include <gloperate/primitives/ScreenAlignedQuad.h>


using namespace gl;
using namespace glm;
using namespace globjects;


AtomicCounter::AtomicCounter(gloperate::ResourceManager & resourceManager, const cpplocate::ModuleInfo & moduleInfo)
:   Painter("AtomicCounter", resourceManager, moduleInfo)
,   m_targetFramebufferCapability(addCapability(new gloperate::TargetFramebufferCapability()))
,   m_viewportCapability(addCapability(new gloperate::ViewportCapability()))
,   m_projectionCapability(addCapability(new gloperate::PerspectiveProjectionCapability(m_viewportCapability)))
,   m_cameraCapability(addCapability(new gloperate::CameraCapability(glm::vec3(-1.f, 0.f, 2.f))))
,   m_typedRenderTargetCapability(addCapability(new gloperate::TypedRenderTargetCapability()))
,   m_frameCount(0)
,   m_choice(GBufferChoice::PostProcessed)
{
    setupPropertyGroup();
    m_targetFramebufferCapability->changed.connect([this](){ this->targetFramebufferChanged();});
}

AtomicCounter::~AtomicCounter() = default;

void AtomicCounter::setupPropertyGroup()
{
    addProperty<GBufferChoice>("gbuffer", this,
        &AtomicCounter::gbufferchoice, &AtomicCounter::setGbufferchoice)->setStrings({
        { GBufferChoice::PostProcessed, "PostProcessed" },
        { GBufferChoice::Color, "Color" },
        { GBufferChoice::Normal, "Normal" },
        { GBufferChoice::WorldCoord, "WorldCoord" },
        { GBufferChoice::Depth, "Depth" }});
}

GBufferChoice AtomicCounter::gbufferchoice() const
{
    return m_choice;
}

void AtomicCounter::setGbufferchoice(GBufferChoice c)
{
    m_choice = c;
}

void AtomicCounter::setupProjection()
{
    static const auto zNear = 1.f, zFar = 16.f, fovy = 50.f;

    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));
}

void AtomicCounter::onInitialize()
{
    // create program
    globjects::init();

#ifdef __APPLE__
    Shader::clearGlobalReplacements();
    Shader::globalReplace("#version 140", "#version 150");

    debug() << "Using global OS X shader replacement '#version 140' -> '#version 150'" << std::endl;
#endif

    glClearColor(1.f, 1.f, 1.f, 0.f);

    setupProjection();

    m_icosahedron = new gloperate::Icosahedron(2);

    m_sphere = new Program();

    m_sphere->attach(
        Shader::fromFile(GL_VERTEX_SHADER,   "data/atomiccounter/sphere.vert"),
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/atomiccounter/sphere.frag"));

    m_colorTexture    = Texture::createDefault(GL_TEXTURE_2D);
    m_depthTexture    = Texture::createDefault(GL_TEXTURE_2D);
    m_normalTexture   = Texture::createDefault(GL_TEXTURE_2D);
    m_geometryTexture = Texture::createDefault(GL_TEXTURE_2D);

    m_sphereFBO = new Framebuffer;
    m_sphereFBO->attachTexture(GL_COLOR_ATTACHMENT0, m_colorTexture);
    m_sphereFBO->attachTexture(GL_COLOR_ATTACHMENT1, m_normalTexture);
    m_sphereFBO->attachTexture(GL_COLOR_ATTACHMENT2, m_geometryTexture);
    m_sphereFBO->attachTexture(GL_DEPTH_ATTACHMENT,  m_depthTexture);
    m_sphereFBO->setDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });

    targetFramebufferChanged();

    m_postprocessing = new gloperate::ScreenAlignedQuad(
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/atomiccounter/postprocessing.frag"));
    m_postprocessing->program()->setUniform<GLint>("colorSource",      0);
    m_postprocessing->program()->setUniform<GLint>("normalSource",     1);
    m_postprocessing->program()->setUniform<GLint>("worldCoordSource", 2);
    m_postprocessing->program()->setUniform<GLint>("depthSource",      3);

    m_postprocessedTexture = Texture::createDefault(GL_TEXTURE_2D);

    m_postprocessingFBO = new Framebuffer;
    m_postprocessingFBO->attachTexture(GL_COLOR_ATTACHMENT0, m_postprocessedTexture);
    m_postprocessingFBO->setDrawBuffer(GL_COLOR_ATTACHMENT0);

    m_gBufferChoice = new gloperate::ScreenAlignedQuad(
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/atomiccounter/gbufferchoice-atomic.frag"));
    m_gBufferChoice->program()->setUniform<GLint>("postprocessedSource", 0);
    m_gBufferChoice->program()->setUniform<GLint>("colorSource",         1);
    m_gBufferChoice->program()->setUniform<GLint>("normalSource",        2);
    m_gBufferChoice->program()->setUniform<GLint>("worldCoordSource",    3);
    m_gBufferChoice->program()->setUniform<GLint>("depthSource",         4);

    m_gBufferChoice->program()->setUniform<GLfloat>("nearZ", m_projectionCapability->zNear());
    m_gBufferChoice->program()->setUniform<GLfloat>("farZ",  m_projectionCapability->zFar());

    m_atomicCounterBuffer = new Buffer();
    m_atomicCounterBuffer->setData(std::array<unsigned int, 1>{{0}}, GL_DYNAMIC_DRAW);

    m_cameraCapability->setEye(glm::vec3(-1.f, 0.f, 2.f));
    cameraChanged();

    glDepthMask(GL_TRUE);
}

void AtomicCounter::viewportChanged()
{
    glViewport(
        m_viewportCapability->x(),
        m_viewportCapability->y(),
        m_viewportCapability->width(),
        m_viewportCapability->height());

    m_postprocessing->program()->setUniform<vec2>("screenSize", vec2(m_viewportCapability->width(), m_viewportCapability->height()));

    m_colorTexture->image2D(0, GL_RGBA8,   m_viewportCapability->width(), m_viewportCapability->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_normalTexture->image2D(0, GL_RGBA16F, m_viewportCapability->width(), m_viewportCapability->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
    m_geometryTexture->image2D(0, GL_RGBA16F, m_viewportCapability->width(), m_viewportCapability->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
    m_depthTexture->image2D(0, GL_DEPTH_COMPONENT, m_viewportCapability->width(), m_viewportCapability->height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    m_postprocessedTexture->image2D(0, GL_RGBA8,   m_viewportCapability->width(), m_viewportCapability->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void AtomicCounter::cameraChanged()
{
    m_sphere->setUniform("transform", m_projectionCapability->projection() * m_cameraCapability->view());
    m_sphere->setUniform("modelView", m_cameraCapability->view());
    m_sphere->setUniform("normalMatrix", m_cameraCapability->normal());
}

void AtomicCounter::targetFramebufferChanged()
{
    m_typedRenderTargetCapability->setRenderTarget(gloperate::RenderTargetType::Depth, m_sphereFBO, gl::GLenum::GL_DEPTH_ATTACHMENT, gl::GLenum::GL_DEPTH_COMPONENT);
}

void AtomicCounter::onPaint()
{
    if (m_viewportCapability->hasChanged())
    {
        viewportChanged();
        m_viewportCapability->setChanged(false);
    }
    
    cameraChanged();

    m_gBufferChoice->program()->setUniform<GLint>("choice", static_cast<unsigned int>(m_choice));


    // Sphere Pass

    m_sphereFBO->bind();

    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sphere->use();
    m_icosahedron->draw();
    m_sphere->release();

    m_sphereFBO->unbind();

    // Postprocessing Pass

    glDisable(GL_DEPTH_TEST);

    m_postprocessingFBO->bind();

    glClear(GL_COLOR_BUFFER_BIT);

    m_colorTexture->bindActive(GL_TEXTURE0);
    m_normalTexture->bindActive(GL_TEXTURE1);
    m_geometryTexture->bindActive(GL_TEXTURE2);
    m_depthTexture->bindActive(GL_TEXTURE3);

    m_postprocessing->draw();

    m_colorTexture->unbindActive(GL_TEXTURE0);
    m_normalTexture->unbindActive(GL_TEXTURE1);
    m_geometryTexture->unbindActive(GL_TEXTURE2);
    m_depthTexture->unbindActive(GL_TEXTURE3);

    m_postprocessingFBO->unbind();

    // GBuffer Choice Pass (including blitting)

    auto fbo = m_targetFramebufferCapability->framebuffer();

    if (!fbo)
        fbo = globjects::Framebuffer::defaultFBO();

    fbo->bind(GL_FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_postprocessedTexture->bindActive(GL_TEXTURE0);
    m_colorTexture->bindActive(GL_TEXTURE1);
    m_normalTexture->bindActive(GL_TEXTURE2);
    m_geometryTexture->bindActive(GL_TEXTURE3);
    m_depthTexture->bindActive(GL_TEXTURE4);

    m_atomicCounterBuffer->setSubData(std::array<unsigned int, 1>{{0}}, 0);
    m_atomicCounterBuffer->bindBase(GL_ATOMIC_COUNTER_BUFFER, 0);

    m_gBufferChoice->program()->setUniform<unsigned int>("frameCount", m_frameCount);
    m_gBufferChoice->draw();

    m_frameCount = (m_frameCount + 100) % (m_viewportCapability->width() * m_viewportCapability->height());

    m_postprocessedTexture->unbindActive(GL_TEXTURE0);
    m_colorTexture->unbindActive(GL_TEXTURE1);
    m_normalTexture->unbindActive(GL_TEXTURE2);
    m_geometryTexture->unbindActive(GL_TEXTURE3);
    m_depthTexture->unbindActive(GL_TEXTURE4);

    Framebuffer::unbind(GL_FRAMEBUFFER);
}
