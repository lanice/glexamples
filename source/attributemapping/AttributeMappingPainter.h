
#pragma once


#include <string>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>

#include "LineGeometry.h"
#include "AttributeStorage.h"


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


class AttributeMappingPainter : public gloperate::Painter
{
public:
    AttributeMappingPainter(gloperate::ResourceManager & resourceManager, const reflectionzeug::Variant & pluginInfo);
    virtual ~AttributeMappingPainter();

    std::string getLineColor() const;
    void setLineColor(const std::string & attr);

    std::string getLineWidth() const;
    void setLineWidth(const std::string & attr);

    std::string getNodeHeight() const;
    void setNodeHeight(const std::string & attr);


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
    globjects::ref_ptr<AttributeStorage>                 m_attrStorage;
    globjects::ref_ptr<globjects::Program>               m_program;

    // Options
    std::string                                          m_lineColor;   ///< Name of attribute that is mapped to color
    std::string                                          m_lineWidth;   ///< Name of attribute that is mapped to width
    std::string                                          m_nodeHeight;  ///< Name of attribute that is mapped to height
};
