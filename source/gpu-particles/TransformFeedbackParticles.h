#pragma once

#include <globjects/base/ref_ptr.h>

#include "AbstractParticleTechnique.h"


namespace globjects
{
class TransformFeedback;
class VertexArray;
class Program;
class Buffer;
}

class TransformFeedbackParticles : public AbstractParticleTechnique
{
public:
    TransformFeedbackParticles(
        const std::vector<glm::vec4> & positions
    ,   const std::vector<glm::vec4> & velocities
    ,   const globjects::Texture & forces
    ,   const gloperate::AbstractCameraCapability & cameraCap
    ,   const glm::ivec2 viewport);
    virtual ~TransformFeedbackParticles();

    virtual void initialize() override;
    virtual void reset() override;

    virtual void step(float elapsed) override;

protected:
    virtual void draw_impl() override;

protected:
    globjects::ref_ptr<globjects::TransformFeedback> m_transformFeedback;
    globjects::ref_ptr<globjects::Program> m_transformFeedbackProgram;

    globjects::ref_ptr<globjects::Buffer> m_sourcePositions;
    globjects::ref_ptr<globjects::Buffer> m_sourceVelocities;
    globjects::ref_ptr<globjects::Buffer> m_targetPositions;
    globjects::ref_ptr<globjects::Buffer> m_targetVelocities;

    globjects::ref_ptr<globjects::VertexArray> m_vao;
};
