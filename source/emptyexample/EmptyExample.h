
#pragma once


#include <string>
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
    EmptyExample(gloperate::ResourceManager & resourceManager, const cpplocate::ModuleInfo & moduleInfo);
    virtual ~EmptyExample();

    void setupProjection();


protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;


protected:
    // Internal data
    std::string m_dataPath;

    // Capabilities
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;

    // OpenGL objects
    globjects::ref_ptr<gloperate::AdaptiveGrid> m_grid;
    globjects::ref_ptr<gloperate::Icosahedron> m_icosahedron;
    globjects::ref_ptr<globjects::Program> m_program;
    gl::GLint m_transformLocation;
};
