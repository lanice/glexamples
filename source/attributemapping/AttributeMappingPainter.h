
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
}

namespace gloperate
{
    class AdaptiveGrid;
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
    class AbstractVirtualTimeCapability;
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

    bool mappingVisible() const;
    void setMappingVisible(bool visible);


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
    gloperate::AbstractVirtualTimeCapability           * m_virtualTimeCapability;

    // Property groups
    reflectionzeug::PropertyGroup                      * m_propLines;       ///< Property group for line rendering options
    reflectionzeug::PropertyGroup                      * m_propMapping;     ///< Property group for attribute mapping options

    // Options
    bool                                                 m_linesVisible;    ///< Render simple lines?
    std::string                                          m_colorMap;        ///< Color map
    std::string                                          m_lineColor;       ///< Name of attribute that is mapped to color
    std::string                                          m_lineWidth;       ///< Name of attribute that is mapped to width
    std::string                                          m_nodeHeight;      ///< Name of attribute that is mapped to height
    bool                                                 m_mappingVisible;  ///< Render attribute mapping?

    // Data
    DataSet                                            * m_dataSet;
    MappingConfigList                                  * m_configs;         ///< Mapping configurations
    std::vector<std::string>                             m_attributes;      ///< List of available attributes
    std::vector<std::string>                             m_colorMaps;       ///< List of available color maps
    std::vector<std::string>                             m_textureMaps;     ///< List of available texture maps

    // Rendering
    globjects::ref_ptr<gloperate::AdaptiveGrid>          m_grid;            ///< Grid renderer
    globjects::ref_ptr<LineGeometry>                     m_lineGeometry;    ///< Dataset representation in the form of points (for line rendering)
    globjects::ref_ptr<NodeGeometry>                     m_nodeGeometry;    ///< Dataset representation in the form of lines  (for attribute mapping)
    globjects::ref_ptr<AttributeStorage>                 m_attrStorage;     ///< GPU representation of attributes
    globjects::ref_ptr<globjects::Program>               m_programLines;    ///< Program for rendering (for line rendering)
    globjects::ref_ptr<globjects::Program>               m_programMapping;  ///< Program for rendering (for attribute mapping)
    globjects::ref_ptr<globjects::Texture>               m_colorMapTexture; ///< Texture containing the current color map (for line rendering)
    globjects::ref_ptr<globjects::Texture>               m_textureMapsTex;  ///< Texture containing all texture maps (for attribute mapping)
    globjects::ref_ptr<globjects::Texture>               m_colorMapsTex;    ///< Texture containing all color maps (for attribute mapping)
    globjects::ref_ptr<globjects::Texture>               m_configDataTex;   ///< Uniform buffer containing the mapping configurations
};
