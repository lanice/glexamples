#include "gpu-particles.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <glbinding/gl/functions.h>
#include <glbinding/gl/extension.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/boolean.h>

#include <globjects/globjects.h>
#include <globjects/logging.h>
#include <globjects/Texture.h>
#include <globjects/NamedString.h>

#include <globjects/base/File.h>

#include <gloperate/base/RenderTargetType.h>

#include <gloperate/painter/TargetFramebufferCapability.h>
#include <gloperate/painter/ViewportCapability.h>
#include <gloperate/painter/PerspectiveProjectionCapability.h>
#include <gloperate/painter/CameraCapability.h>
#include <gloperate/painter/VirtualTimeCapability.h>

#include "AbstractParticleTechnique.h"

#include "ComputeShaderParticles.h"
#include "FragmentShaderParticles.h"
#include "TransformFeedbackParticles.h"


using namespace gl;
using namespace glm;
using namespace globjects;
using namespace gloperate;


GpuParticles::GpuParticles(ResourceManager & resourceManager, const cpplocate::ModuleInfo & moduleInfo)
:   Painter("gpu-particles", resourceManager, moduleInfo)
,   m_targetFramebufferCapability(addCapability(new TargetFramebufferCapability()))
,   m_viewportCapability(addCapability(new ViewportCapability()))
,   m_projectionCapability(addCapability(new PerspectiveProjectionCapability(m_viewportCapability)))
,   m_cameraCapability(addCapability(new CameraCapability(vec3(0.f, 1.f,-3.f))))
,   m_technique(ParticleTechnique::FragmentShaderTechnique)
,   m_numParticles(262144)
,   m_steps(1)
{
    setupPropertyGroup();
    m_timer.setAutoUpdating(false);
    m_timer.start();
}

GpuParticles::~GpuParticles() = default;

void GpuParticles::setupPropertyGroup()
{
    addProperty<ParticleTechnique>("technique", this,
        &GpuParticles::particleTechnique, &GpuParticles::setParticleTechnique)->setStrings({
        { ParticleTechnique::ComputeShaderTechnique, "Compute Shader Technique" },
        { ParticleTechnique::TransformFeedbackTechnique, "Transform Feedback Technique" },
        { ParticleTechnique::FragmentShaderTechnique, "Fragment Shader Technique" }});
}

void GpuParticles::setupProjection()
{
    static const auto zNear = 1.f, zFar = 16.f, fovy = 50.f;

    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));
}

GpuParticles::ParticleTechnique GpuParticles::particleTechnique() const
{
    return m_technique;
}

void GpuParticles::setParticleTechnique(ParticleTechnique technique)
{
    if (m_techniques.count(technique) > 0)
    {
        m_technique = technique;
        debug() << "Switched to " << property<ParticleTechnique>("technique")->toString();
    }
}

void GpuParticles::onInitialize()
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


    // Initialize Particle Positions and Attributes

    m_positions.resize(m_numParticles);
    for (int i = 0; i < m_numParticles; ++i)
        m_positions[i] = vec4(sphericalRand<float>(1.f), 1.f);

    m_velocities.resize(m_numParticles);
    for (int i = 0; i < m_numParticles; ++i)
        m_velocities[i] = vec4(0.f);


    m_forces = Texture::createDefault(GL_TEXTURE_3D);
    
    NamedString::create("/particle-step.inc", new File("data/gpu-particles/particle-step.inc"));


    // initialize techniques

    if (hasExtension(GLextension::GL_ARB_compute_shader))
        m_techniques[ParticleTechnique::ComputeShaderTechnique] = new ComputeShaderParticles(
            m_positions, m_velocities, *m_forces, *m_cameraCapability, ivec2(m_viewportCapability->width(), m_viewportCapability->height()));
    else
        warning() << "Compute shader based implementation not supported.";

    if (hasExtension(GLextension::GL_ARB_transform_feedback3)) 
        m_techniques[ParticleTechnique::TransformFeedbackTechnique] = new TransformFeedbackParticles(
            m_positions, m_velocities, *m_forces, *m_cameraCapability, ivec2(m_viewportCapability->width(), m_viewportCapability->height()));
    else
        warning() << "Transform feedback based implementation not supported.";

    m_techniques[ParticleTechnique::FragmentShaderTechnique] = new FragmentShaderParticles(
        m_positions, m_velocities, *m_forces, *m_cameraCapability, ivec2(m_viewportCapability->width(), m_viewportCapability->height()));

    for (auto technique : m_techniques)
        if (technique.second)
            technique.second->initialize();

    reset();
    

    m_cameraCapability->setEye(vec3(0.f, 1.f,-3.f));
    cameraChanged();

    glDepthMask(GL_TRUE);
}

void GpuParticles::cameraChanged()
{
    
}

void GpuParticles::onPaint()
{
    if (m_viewportCapability->hasChanged())
    {
        glViewport(
            m_viewportCapability->x(),
            m_viewportCapability->y(),
            m_viewportCapability->width(),
            m_viewportCapability->height());

        for (auto technique : m_techniques)
            if (technique.second)
            {
                technique.second->setViewport(ivec2(m_viewportCapability->width(), m_viewportCapability->height()));
                technique.second->resize();
            }

        m_viewportCapability->setChanged(false);
    }

    const long double elapsed = static_cast<long double>(m_timer.elapsed().count());
    m_timer.update();

    const float delta = static_cast<float>((m_timer.elapsed().count() - elapsed) * 1.0e-9L);

    glEnable(GL_DEPTH_TEST);

    step(delta); // requires context to be current
    m_techniques[m_technique]->draw(delta, m_projectionCapability->projection());

    glDisable(GL_DEPTH_TEST);

    cameraChanged();
}

void GpuParticles::step(const float delta)
{
    const float delta_stepped = delta / static_cast<float>(m_steps);

    for (int i = 0; i < m_steps; ++i)
        m_techniques[m_technique]->step(delta_stepped);
}

void GpuParticles::reset(const bool particles)
{
    // initialize 3D Force Field (3D Texture)

    static const ivec3 fdim(5, 5, 5); // this has center axises and allows for random rings etc..

    std::vector<vec3> forces;
    forces.resize(fdim.x * fdim.y * fdim.z);

    srand(static_cast<unsigned int>(time(0)));

    for (int z = 0; z < fdim.z; ++z)
    for (int y = 0; y < fdim.y; ++y)
    for (int x = 0; x < fdim.x; ++x)
    {
        const int i = z *  fdim.x * fdim.y + y * fdim.x + x;
        const vec3 f(sphericalRand<float>(1.0));

        forces[i] = f * (1.f - length(vec3(x, y, z)) / std::sqrt(3.f));
    }

    m_forces->image3D(0, GL_RGB32F, fdim.x, fdim.y, fdim.z, 0, GL_RGB, GL_FLOAT, forces.data());

    if (!particles)
        return;

    m_timer.reset();
    m_timer.update();

    for (auto technique : m_techniques)
        technique.second->reset();
}
