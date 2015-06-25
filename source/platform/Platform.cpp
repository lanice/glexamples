#include "Platform.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>

#include <globjects/globjects.h>
#include <globjects/logging.h>
#include <globjects/DebugMessage.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>

#include <gloperate/base/RenderTargetType.h>

#include <gloperate/painter/TargetFramebufferCapability.h>
#include <gloperate/painter/ViewportCapability.h>
#include <gloperate/painter/PerspectiveProjectionCapability.h>
#include <gloperate/painter/CameraCapability.h>
#include <gloperate/painter/TypedRenderTargetCapability.h>
#include <gloperate/painter/VirtualTimeCapability.h>

#include <gloperate/primitives/AdaptiveGrid.h>

#include "hitmanbox/HitmanBox.h"
#include "RawFile.h"

using namespace gl;
using namespace glm;
using namespace globjects;


Platform::Platform(gloperate::ResourceManager & resourceManager)
:   Painter(resourceManager)
,   m_targetFramebufferCapability(addCapability(new gloperate::TargetFramebufferCapability()))
,   m_viewportCapability(addCapability(new gloperate::ViewportCapability()))
,   m_projectionCapability(addCapability(new gloperate::PerspectiveProjectionCapability(m_viewportCapability)))
,   m_typedRenderTargetCapability(addCapability(new gloperate::TypedRenderTargetCapability()))
,   m_cameraCapability(addCapability(new gloperate::CameraCapability()))
,   m_timeCapability(addCapability(new gloperate::VirtualTimeCapability()))
{
    m_timeCapability->setLoopDuration(20.0f * pi<float>());

    m_targetFramebufferCapability->changed.connect([this](){ this->onTargetFramebufferChanged();});
}

Platform::~Platform()
{
}

void Platform::setupProjection()
{
    static const auto zNear = 0.3f, zFar = 15.f, fovy = 50.f;

    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));

    m_grid->setNearFar(zNear, zFar);
}

void Platform::onInitialize()
{
    // create program

    globjects::init();
    onTargetFramebufferChanged();

#ifdef __APPLE__
    Shader::clearGlobalReplacements();
    Shader::globalReplace("#version 140", "#version 150");

    debug() << "Using global OS X shader replacement '#version 140' -> '#version 150'" << std::endl;
#endif

    m_grid = new gloperate::AdaptiveGrid{};
    m_grid->setColor({0.6f, 0.6f, 0.6f});

    RawFile raw("data/platform/wood-light.3096.2048.rgba.ub.raw");
    m_boxTexture = Texture::createDefault(GL_TEXTURE_2D);
    m_boxTexture->image2D(0, GL_RGBA8, ivec2(3096, 2048), 0, GL_RGBA, GL_UNSIGNED_BYTE, raw.data());
    m_boxTexture->setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    m_platform = new HitmanBox();

    m_program = new Program{};
    m_program->attach(
        Shader::fromFile(GL_VERTEX_SHADER, "data/platform/box.vert"),
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/platform/box.frag")
    );

    m_transformLocation = m_program->getUniformLocation("transform");

    glClearColor(0.85f, 0.87f, 0.91f, 1.0f);

    setupProjection();
}

void Platform::onPaint()
{
    if (m_viewportCapability->hasChanged())
    {
        glViewport(
            m_viewportCapability->x(),
            m_viewportCapability->y(),
            m_viewportCapability->width(),
            m_viewportCapability->height());

        m_viewportCapability->setChanged(false);
    }

    auto fbo = m_targetFramebufferCapability->framebuffer();

    if (!fbo)
        fbo = globjects::Framebuffer::defaultFBO();

    fbo->bind(GL_FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    const auto transform = m_projectionCapability->projection() * m_cameraCapability->view();
    const auto eye = m_cameraCapability->eye();

    m_grid->update(eye, transform);
    m_grid->draw();

    m_program->use();
    m_program->setUniform(m_transformLocation, transform);

    m_boxTexture->bindActive(GL_TEXTURE0);
    m_platform->draw();
    m_boxTexture->unbindActive(GL_TEXTURE0);

    m_program->release();

    Framebuffer::unbind(GL_FRAMEBUFFER);
}

void Platform::onTargetFramebufferChanged()
{
    auto fbo = m_targetFramebufferCapability->framebuffer();

    if (!fbo)
        fbo = globjects::Framebuffer::defaultFBO();

    m_typedRenderTargetCapability->setRenderTarget(gloperate::RenderTargetType::Depth, fbo,
        GLenum::GL_DEPTH_ATTACHMENT, GLenum::GL_DEPTH_COMPONENT);
}
