#pragma once

#include <memory>

#include <glbinding/gl/types.h>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>


namespace globjects
{
    class Program;
}

namespace gloperate
{
    class AdaptiveGrid;
    class Icosahedron;
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
}

class EmptyExample : public gloperate::Painter
{
public:
    EmptyExample(gloperate::ResourceManager & resourceManager);
    virtual ~EmptyExample();

    void setupProjection();

protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;

protected:
    /* capabilities */
    std::unique_ptr<gloperate::AbstractTargetFramebufferCapability> m_targetFramebufferCapability;
    std::unique_ptr<gloperate::AbstractViewportCapability> m_viewportCapability;
    std::unique_ptr<gloperate::AbstractPerspectiveProjectionCapability> m_projectionCapability;
    std::unique_ptr<gloperate::AbstractCameraCapability> m_cameraCapability;

    /* members */
    globjects::ref_ptr<gloperate::AdaptiveGrid> m_grid;
    globjects::ref_ptr<gloperate::Icosahedron> m_icosahedron;
    globjects::ref_ptr<globjects::Program> m_program;
    gl::GLint m_transformLocation;
};
