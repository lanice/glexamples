
#pragma once


#include <vector>
#include <string>

#include <glm/glm.hpp>
 
#include <reflectionzeug/Object.h>

#include "attributemapping_api.h"


class MappingConfigList;


typedef float MappingValue;


/**
*  @brief
*    A configuration for the real-time mapping of attributes
*/
class ATTRIBUTEMAPPING_API MappingConfig : public reflectionzeug::Object
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] configList
    *    The owning configuration list
    *  @param[in] useLod
    *    if 'true', 4 LOD configurations will be created for this config
    */
    MappingConfig(MappingConfigList * configList, bool useLod = true, std::string name = "Config");

    /**
    *  @brief
    *    Destructor
    */
    ~MappingConfig();

    /**
    *  @brief
    *    Set available attributes for mapping
    *
    *  @param[in] attributes
    *    List of available attributes
    */
    void setAttributes(const std::vector<std::string> & attributes);

    /**
    *  @brief
    *    Check if configuration has been updated
    *
    *  @return
    *    'true' if the configuration has been updated, else 'false'
    *
    *  @remarks
    *    This function call also resets the updated-flag to 'false'
    */
    bool checkUpdated();

    /**
    *  @brief
    *    Get style preset
    *
    *  @return
    *    Preset name
    */
    std::string getStyle() const;

    /**
    *  @brief
    *    Set style preset
    *
    *  @param[in] style
    *    Preset name
    */
    void setStyle(const std::string & style);

    /**
    *  @brief
    *    Get configuration for LOD
    *
    *  @param[in] lod
    *    LOD level (1..4)
    *
    *  @return
    *    Pointer to the configuration, this on error
    *
    *  @remarks
    *    LOD level 1..3 depend on depth (1==near, 3==far),
    *    LOD level 4 stands for the currently highlighted object
    */
    MappingConfig * getLodConfig(unsigned int lod) const;

    int textureID() const;
    void setTextureID(int id);

    std::string texture() const;
    void setTexture(const std::string & texture);

    int colorMapID() const;
    void setColorMapID(int id);

    std::string colorMap() const;
    void setColorMap(const std::string & colorMap);

    int geometryType() const;
    void setGeometryType(int type);

    int tesselation() const;
    void setTesselation(int tesselation);

    float minRadius() const;
    void setMinRadius(float radius);

    float maxRadius() const;
    void setMaxRadius(float radius);

    float stretchSize() const;
    void setStretchSize(float size);

    float torsionSize() const;
    void setTorsionSize(float size);

    float animationSpeed() const;
    void setAnimationSpeed(float speed);

    glm::vec3 upVector() const;
    void setUpVector(const glm::vec3 & up);

    MappingValue positionX() const;
    void setPositionX(MappingValue x);

    MappingValue positionY() const;
    void setPositionY(MappingValue y);

    MappingValue positionZ() const;
    void setPositionZ(MappingValue z);

    MappingValue radius() const;
    void setRadius(MappingValue radius);

    MappingValue color() const;
    void setColor(MappingValue color);

    MappingValue textureX() const;
    void setTextureX(MappingValue textureX);

    MappingValue textureY() const;
    void setTextureY(MappingValue textureY);

    MappingValue alpha() const;
    void setAlpha(MappingValue alpha);


protected:
    // Data
    MappingConfigList      * m_configList;       ///< The owning configuration list
    std::vector<std::string> m_attributes;       ///< List of available attributes
    bool                     m_useLod;           ///< Use LOD in mapping?
    bool                     m_updated;          ///< Has the configuration been updated?
    std::string              m_style;            ///< The currently used style preset

    // Static configuration values
    int                      m_textureID;        ///< ID of used texture
    std::string              m_texture;          ///< Texture
    int                      m_colorMapID;       ///< ID of used color map
    std::string              m_colorMap;         ///< Color map
    int                      m_geometryType;     ///< Geometry type: 0 = discard, 1 = tubes, 2 = sphere
    int                      m_tesselation;      ///< Number of corners of each polygon
    float                    m_minRadius;        ///< Minimal radius
    float                    m_maxRadius;        ///< Maximal radius
    float                    m_stretchSize;      ///< Length of a texture repetition with single stretching
    float                    m_torsionSize;      ///< Value change leading to a complete 360° texture distortion
    float                    m_animationSpeed;   ///< Animation speed of the texture (texture repetitions/second)
    glm::vec3                m_upVector;         ///< Global up-vector

    // Mappable values
    MappingValue             m_positionX;        ///< Position coordinate X
    MappingValue             m_positionY;        ///< Position coordinate Y
    MappingValue             m_positionZ;        ///< Position coordinate Z
    MappingValue             m_radius;           ///< Tube radius
    MappingValue             m_color;            ///< Color
    MappingValue             m_textureX;         ///< Texture coordinate X
    MappingValue             m_textureY;         ///< Texture coordinate Y
    MappingValue             m_alpha;            ///< Alpha value (transparency)
};
