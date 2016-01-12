#pragma once

#include <memory>

#include <glbinding/gl/types.h>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>


namespace globjects
{
    class Program;
    class Texture;
    class Framebuffer;
    class Buffer;
}

namespace gloperate
{
    class Icosahedron;
    class ScreenAlignedQuad;
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
    class TypedRenderTargetCapability;
}


enum class GBufferChoice { PostProcessed, Color, Normal, WorldCoord, Depth };


class AtomicCounter : public gloperate::Painter
{
public:
    AtomicCounter(gloperate::ResourceManager & resourceManager, const cpplocate::ModuleInfo & moduleInfo);
    virtual ~AtomicCounter();

    void cameraChanged();
    void setupProjection();
    void viewportChanged();
    void targetFramebufferChanged();

    GBufferChoice gbufferchoice() const;
    void setGbufferchoice(GBufferChoice c);

protected:
    void setupPropertyGroup();
    virtual void onInitialize() override;
    virtual void onPaint() override;

protected:
    /* capabilities */
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;
    gloperate::TypedRenderTargetCapability * m_typedRenderTargetCapability;

    /* members */
    globjects::ref_ptr<gloperate::Icosahedron> m_icosahedron;
    globjects::ref_ptr<globjects::Program> m_sphere;
    globjects::ref_ptr<globjects::Texture> m_colorTexture;
    globjects::ref_ptr<globjects::Texture> m_normalTexture;
    globjects::ref_ptr<globjects::Texture> m_geometryTexture;
    globjects::ref_ptr<globjects::Texture> m_depthTexture;
    globjects::ref_ptr<globjects::Framebuffer> m_sphereFBO;

    globjects::ref_ptr<gloperate::ScreenAlignedQuad> m_postprocessing;
    globjects::ref_ptr<globjects::Texture> m_postprocessedTexture;
    globjects::ref_ptr<globjects::Framebuffer> m_postprocessingFBO;

    globjects::ref_ptr<gloperate::ScreenAlignedQuad> m_gBufferChoice;

    globjects::ref_ptr<globjects::Buffer> m_atomicCounterBuffer;
    unsigned int m_frameCount;
    
    GBufferChoice m_choice;
};
