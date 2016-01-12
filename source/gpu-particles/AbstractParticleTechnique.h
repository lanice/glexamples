#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <globjects/base/ref_ptr.h>


namespace globjects
{
    class Framebuffer;
    class Program;
    class Buffer;
    class Texture;
}

namespace gloperate
{
    class AbstractCameraCapability;
    class ScreenAlignedQuad;
}



class AbstractParticleTechnique
{
public:
    AbstractParticleTechnique(
        const std::vector<glm::vec4> & postions
    ,   const std::vector<glm::vec4> & velocities
    ,   const globjects::Texture & forces
    ,   const gloperate::AbstractCameraCapability & cameraCap
    ,   const glm::ivec2 viewport);

    virtual ~AbstractParticleTechnique();

    virtual void initialize() = 0;
    virtual void reset();

    virtual void draw(float elapsed, const glm::mat4 projection);
    virtual void step(float elapsed) = 0;

    void resize();

    void pause(bool paused);
    
    void setViewport(glm::ivec2 viewport);

    // Note: this is intentionally not implemented - but fixes MSVC12 C4512 warning
    AbstractParticleTechnique & operator=(const AbstractParticleTechnique & particleTechnique);

protected:
    void initialize(const std::string & vertexShaderSourceFilePath);
    virtual void draw_impl() = 0; // // use m_drawProgram


protected:
    const std::vector<glm::vec4> m_positions;
    const std::vector<glm::vec4> m_velocities;

    const globjects::Texture & m_forces;
    const gloperate::AbstractCameraCapability & m_cameraCap;

    const unsigned int m_numParticles;

    bool m_paused;

    glm::ivec2 m_viewport;


    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    globjects::ref_ptr<globjects::Texture> m_color;

    globjects::ref_ptr<globjects::Program> m_drawProgram;

    globjects::ref_ptr<gloperate::ScreenAlignedQuad> m_quad;
    globjects::ref_ptr<gloperate::ScreenAlignedQuad> m_clear;
};
