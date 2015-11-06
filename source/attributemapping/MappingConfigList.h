
#pragma once


#include <vector>
#include <string>

#include <reflectionzeug/Object.h>
#include <reflectionzeug/variant/Variant.h>

#include "attributemapping_api.h"


class MappingConfig;


/**
*  @brief
*    List of configurations available for real-time mapping
*/
class ATTRIBUTEMAPPING_API MappingConfigList : public reflectionzeug::Object
{
public:
    /**
    *  @brief
    *    Constructor
    */
    MappingConfigList();

    /**
    *  @brief
    *    Destructor
    */
    ~MappingConfigList();

    /**
    *  @brief
    *    Get visualization preset
    *
    *  @return
    *    Preset name
    */
    std::string getPreset() const;

    /**
    *  @brief
    *    Set visualization preset
    *
    *  @param[in] preset
    *    Preset name
    */
    void setPreset(const std::string & preset);

    /**
    *  @brief
    *    Get classification
    *
    *  @return
    *    Name of attribute
    */
    std::string classification() const;

    /**
    *  @brief
    *    Set classification
    *
    *  @param[in] classification
    *    Name of attribute
    */
    void setClassification(const std::string & classification);

    /**
    *  @brief
    *    Get classification index
    *
    *  @return
    *    Index of selected attribute, or -1 for focus+context
    */
    int classificationIndex() const;

    /**
    *  @brief
    *    Get space-time-cube
    *
    *  @return
    *    'true' if active, else 'false'
    */
    bool spaceTimeCube() const;

    /**
    *  @brief
    *    Set space-time-cube
    *
    *  @param[in] spaceTimeCube
    *    'true' if active, else 'false'
    */
    void setSpaceTimeCube(bool spaceTimeCube);

    /**
    *  @brief
    *    Get Space-Time Cube interpolation factor
    *
    *  @return
    *    Interpolation factor
    */
    float stcFactor() const;

    /**
    *  @brief
    *    Set Space-Time Cube interpolation factor
    *
    *  @param[in] value
    *    Interpolation factor
    */
    void setStcFactor(float value);

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
    *    Get configuration
    *
    *  @param[in] index
    *    Config index
    *
    *  @return
    *    Pointer to the configuration, nullptr on error
    */
    MappingConfig * getConfig(unsigned int index) const;

    /**
    *  @brief
    *    Get number of configurations
    *
    *  @return
    *    Number of configurations (without LOD)
    */
    unsigned int numConfigs() const;

    /**
    *  @brief
    *    Set number of configurations
    *
    *  @param[in] numConfigs
    *    Number of configurations (without LOD)
    */
    void setNumConfigs(unsigned int numConfigs);

    /**
    *  @brief
    *    Get style preset
    *
    *  @param[in] name
    *    Preset name
    *
    *  @return
    *    Style configuration
    */
    reflectionzeug::Variant stylePreset(const std::string & name) const;

    /**
    *  @brief
    *    Set style preset
    *
    *  @param[in] name
    *    Preset name
    *  @param[in] config
    *    Style configuration
    */
    void setStylePreset(const std::string & name, const reflectionzeug::Variant & config);

    /**
    *  @brief
    *    Get configuration preset
    *
    *  @param[in] name
    *    Preset name
    *
    *  @return
    *    Configuration
    */
    reflectionzeug::Variant configPreset(const std::string & name) const;

    /**
    *  @brief
    *    Set configuration preset
    *
    *  @param[in] name
    *    Preset name
    *  @param[in] config
    *    Configuration
    */
    void setConfigPreset(const std::string & name, const reflectionzeug::Variant & config);

    /**
    *  @brief
    *    Get available attributes for mapping
    *
    *  @return
    *    List of available attributes
    */
    const std::vector<std::string> & attributes() const;

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
    *    Get available color maps
    *
    *  @return
    *    List of available color maps
    */
    const std::vector<std::string> & colorMaps() const;

    /**
    *  @brief
    *    Set available color maps
    *
    *  @param[in] colorMaps
    *    List of available color maps
    */
    void setColorMaps(const std::vector<std::string> & colorMaps);

    /**
    *  @brief
    *    Get available texture maps
    *
    *  @return
    *    List of available texture maps
    */
    const std::vector<std::string> & textureMaps() const;

    /**
    *  @brief
    *    Set available texture maps
    *
    *  @param[in] textureMaps
    *    List of available texture maps
    */
    void setTextureMaps(const std::vector<std::string> & textureMaps);


protected:
    std::vector<MappingConfig*> m_configs;        ///< List of configurations
    reflectionzeug::Variant     m_styles;         ///< Style presets
    reflectionzeug::Variant     m_presets;        ///< Config presets
    std::string                 m_preset;         ///< The currently used preset
    std::string                 m_classification; ///< Name of attribute used for classification
    bool                        m_spaceTimeCube;  ///< Use space-time-cube (map time to height)?
    float                       m_stcFactor;      ///< Space-time-cube factor
    std::vector<std::string>    m_attributes;     ///< List of available attributes
    std::vector<std::string>    m_colorMaps;
    std::vector<std::string>    m_textureMaps;
};
