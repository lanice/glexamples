#pragma once

#include <memory>

#include <glbinding/gl/types.h>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>
#include <gloperate/base/ChronoTimer.h>


namespace globjects
{
    class Texture;
}

namespace gloperate
{
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
}

class AbstractParticleTechnique;


class GpuParticles : public gloperate::Painter
{
protected:
    enum class ParticleTechnique { ComputeShaderTechnique, FragmentShaderTechnique, TransformFeedbackTechnique };

public:
    GpuParticles(gloperate::ResourceManager & resourceManager, const cpplocate::ModuleInfo & moduleInfo);
    virtual ~GpuParticles();

    void cameraChanged();
    void setupProjection();

    ParticleTechnique particleTechnique() const;
    void setParticleTechnique(ParticleTechnique technique);

protected:
    void setupPropertyGroup();
    virtual void onInitialize() override;
    virtual void onPaint() override;

    void step(const float delta);
    void reset(const bool particles = true);

protected:
    /* capabilities */
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;

    /* members */
    ParticleTechnique m_technique;
    std::map<ParticleTechnique, AbstractParticleTechnique *> m_techniques;

    // globjects::ref_ptr<gloperate::ChronoTimer> m_timer;
    gloperate::ChronoTimer m_timer;

    int m_numParticles;

    std::vector<glm::vec4> m_positions;
    std::vector<glm::vec4> m_velocities;

    int m_steps;

    struct Attribute
    {
        int moep;
    };
    std::vector<Attribute> m_attributes;

    globjects::ref_ptr<globjects::Texture> m_forces;
};
