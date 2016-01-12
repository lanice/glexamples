#include "AbstractParticleTechnique.h"

#include <cassert>

#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/matrix_inverse.hpp>

#include <glbinding/gl/gl.h>

#include <globjects/Texture.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/Buffer.h>
#include <globjects/Framebuffer.h>

#include <globjects/base/File.h>

#include <gloperate/primitives/ScreenAlignedQuad.h>
#include <gloperate/painter/CameraCapability.h>


using namespace gl;
using namespace glm;
using namespace globjects;
using namespace gloperate;

AbstractParticleTechnique::AbstractParticleTechnique(
    const std::vector<vec4> & positions
,   const std::vector<vec4> & velocities
,   const Texture & forces
,   const AbstractCameraCapability & cameraCap
,   const ivec2 viewport)
: m_positions(positions)
, m_velocities(velocities)
, m_forces(forces)
, m_cameraCap(cameraCap)
, m_numParticles(static_cast<unsigned>(positions.size()))
, m_paused(false)
, m_viewport(viewport)
{
    assert(positions.size() == velocities.size());
}

AbstractParticleTechnique::~AbstractParticleTechnique()
{
}

void AbstractParticleTechnique::initialize(const std::string & vertexShaderSourceFilePath)
{
    m_fbo = new Framebuffer();

    m_color = new Texture(GL_TEXTURE_2D);
    m_color->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_color->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_color->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_color->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_color->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //m_color->image2D(0, GL_RGB16F, glm::ivec2(1, 1), 0, GL_RGB, GL_FLOAT, nullptr);
    m_color->image2D(0, GL_RGBA32F, glm::ivec2(1, 1), 0, GL_RGB, GL_FLOAT, nullptr);

    m_fbo->bind();
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_color);
    m_fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });
    m_fbo->unbind();


    m_clear = new ScreenAlignedQuad(
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/gpu-particles/clear.frag"));
    m_quad = new ScreenAlignedQuad(m_color);

    m_drawProgram = new Program();
    m_drawProgram->attach(
        Shader::fromFile(GL_VERTEX_SHADER, vertexShaderSourceFilePath)
      , Shader::fromFile(GL_GEOMETRY_SHADER, "data/gpu-particles/points.geom")
      , Shader::fromFile(GL_FRAGMENT_SHADER, "data/gpu-particles/points.frag"));

    // try to provide a particle alpha that scales well for 10 as well as for 1000000 particles
    const float alpha = 1.0f - 0.9f * (glm::sqrt(glm::sqrt(static_cast<float>(m_numParticles))) / 30.f);
    m_drawProgram->setUniform("alpha", alpha);
}

void AbstractParticleTechnique::pause(bool paused)
{
    m_paused = paused;
    m_drawProgram->setUniform("paused", paused);
}

void AbstractParticleTechnique::resize()
{
    m_drawProgram->setUniform("aspect", static_cast<float>(m_viewport.x) / max(static_cast<float>(m_viewport.y), 1.f));

    //m_color->image2D(0, GL_RGB16F, m_viewport, 0, GL_RGB, GL_FLOAT, nullptr);
    m_color->image2D(0, GL_RGBA32F, m_viewport, 0, GL_RGB, GL_FLOAT, nullptr);

    m_fbo->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_fbo->unbind();
}

void AbstractParticleTechnique::reset()
{
    if (!m_fbo)
        return;

    m_fbo->bind();
    m_fbo->printStatus(true);
    glClear(GL_COLOR_BUFFER_BIT);
    m_fbo->unbind();
}

void AbstractParticleTechnique::draw(const float elapsed, const mat4 projection)
{
    glDisable(GL_DEPTH_TEST);

    m_fbo->bind();

    glEnable(GL_BLEND);
    if (m_paused)
        // clear color buffer
        glClear(GL_COLOR_BUFFER_BIT);
    else
    {
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

        m_clear->program()->setUniform("elapsed", elapsed);
        m_clear->draw();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // draw particles
    m_drawProgram->setUniform("viewProjection", projection * m_cameraCap.view());

    draw_impl(); // uses m_drawProgram

    glDisable(GL_BLEND);

    m_fbo->unbind();

    m_quad->draw();

    glEnable(GL_DEPTH_TEST);

	glFinish();
}

void AbstractParticleTechnique::setViewport(ivec2 viewport)
{
    m_viewport = viewport;
}
