
#pragma once


#include <vector>
#include <string>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>

#include "LineGeometry.h"
#include "NodeGeometry.h"
#include "AttributeStorage.h"


namespace globjects
{
    class Program;
    class Texture;
    class Buffer;
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
class MappingConfigList;


class ATTRIBUTEMAPPING_API AttributeMappingPainter : public gloperate::Painter
{
public:
    AttributeMappingPainter(gloperate::ResourceManager & resourceManager, const reflectionzeug::Variant & pluginInfo);
    virtual ~AttributeMappingPainter();

    bool linesVisible() const;
    void setLinesVisible(bool visible);

    std::string getColorMap() const;
    void setColorMap(const std::string & colorMap);

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
    void createTextureMaps();
    void createColorMaps();
    void uploadConfig();


protected:
    // Capabilities
    gloperate::AbstractTargetFramebufferCapability     * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability              * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability                * m_cameraCapability;

    // Options
    bool                                                 m_linesVisible;    ///< Render simple lines?
    std::string                                          m_colorMap;        ///< Color map
    std::string                                          m_lineColor;       ///< Name of attribute that is mapped to color
    std::string                                          m_lineWidth;       ///< Name of attribute that is mapped to width
    std::string                                          m_nodeHeight;      ///< Name of attribute that is mapped to height

    // Data
    DataSet                                            * m_dataSet;
    MappingConfigList                                  * m_configs;         ///< Mapping configurations
    std::vector<std::string>                             m_attributes;      ///< List of available attributes
    std::vector<std::string>                             m_colorMaps;       ///< List of available color maps
    std::vector<std::string>                             m_textureMaps;     ///< List of available texture maps

    // Rendering
    globjects::ref_ptr<gloperate::AdaptiveGrid>          m_grid;
    globjects::ref_ptr<LineGeometry>                     m_lineGeometry;
    globjects::ref_ptr<NodeGeometry>                     m_nodeGeometry;
    globjects::ref_ptr<AttributeStorage>                 m_attrStorage;
    globjects::ref_ptr<globjects::Program>               m_program;
    globjects::ref_ptr<globjects::Texture>               m_colorMapTexture;
    globjects::ref_ptr<globjects::Texture>               m_textureMapsTex;
    globjects::ref_ptr<globjects::Texture>               m_colorMapsTex;
    globjects::ref_ptr<globjects::Buffer>                m_configData;      ///< Uniform buffer containing the mapping configurations
};
