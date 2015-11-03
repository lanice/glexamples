
#pragma once


#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>

#include "LineGeometry.h"


namespace globjects
{
    class Program;
}

namespace gloperate
{
    class AdaptiveGrid;
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
}

class DataSet;


class AttributeMapping : public gloperate::Painter
{
public:
    AttributeMapping(gloperate::ResourceManager & resourceManager, const reflectionzeug::Variant & pluginInfo);
    virtual ~AttributeMapping();


protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;

    void generateTestData();


protected:
    // Capabilities
    gloperate::AbstractTargetFramebufferCapability     * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability              * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability                * m_cameraCapability;

    // Data
    DataSet                                            * m_dataSet;

    // Rendering
    globjects::ref_ptr<gloperate::AdaptiveGrid>          m_grid;
    globjects::ref_ptr<LineGeometry>                     m_lineGeometry;
    globjects::ref_ptr<globjects::Program>               m_program;
};
