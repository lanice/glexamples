
#include "MappingConfigList.h"

#include "MappingConfig.h"
#include "Tools.h"
#include "PropertyExtensions.h"


void copyConfig(MappingConfig * config, const reflectionzeug::Variant & values);


MappingConfigList::MappingConfigList()
: reflectionzeug::Object("MappingConfig")
, m_styles(reflectionzeug::Variant::map())
, m_presets(reflectionzeug::Variant::map())
, m_preset("Default")
, m_spaceTimeCube(false)
, m_stcFactor(1.0f)
{
    // Register properties
    addProperty<std::string>("Preset", this, &MappingConfigList::getPreset, &MappingConfigList::setPreset);
    std::vector<std::string> presets = { "Default" };
    PropertyGroup::property("Preset")->setOption("choices", presets);

    addProperty<std::string>("Classification", this, &MappingConfigList::classification, &MappingConfigList::setClassification);
    PropertyGroup::property("Classification")->setOption("choices", m_attributes);

    addProperty<bool>("SpaceTimeCube", this, &MappingConfigList::spaceTimeCube, &MappingConfigList::setSpaceTimeCube);

    addProperty<float>("STCFactor", this, &MappingConfigList::stcFactor, &MappingConfigList::setStcFactor);

    // Register functions
    this->addFunction("setStylePreset",  this, &MappingConfigList::setStylePreset);
    this->addFunction("setConfigPreset", this, &MappingConfigList::setConfigPreset);

    // Register default styles
    reflectionzeug::Variant styleDefault = reflectionzeug::Variant::map();
    reflectionzeug::VariantMap & map = *styleDefault.asMap();
    map["TextureID"]      =       1;
    map["ColorMapID"]     =      19;
    map["GeometryType"]   =       1;
    map["Tesselation"]    =       6;
    map["MinRadius"]      =  100.0f;
    map["MaxRadius"]      =  400.0f;
    map["StretchSize"]    =    8.0f;
    map["TorsionSize"]    =    8.0f;
    map["AnimationSpeed"] =    0.7f;
    map["PositionX"]      =      -3;
    map["PositionY"]      =      -4;
    map["PositionZ"]      =      -5;
    map["Radius"]         =    0.0f;
    map["Color"]          =     -10;
    map["TextureX"]       =      -6;
    map["TextureY"]       =    0.0f;
    map["Alpha"]          =    1.0f;
    setStylePreset("Default", styleDefault);
}

MappingConfigList::~MappingConfigList()
{
}

std::string MappingConfigList::getPreset() const
{
    return m_preset;
}

void MappingConfigList::setPreset(const std::string & preset)
{
    m_preset = preset;

    // Get preset configuration
    reflectionzeug::Variant presetConfig = (*m_presets.asMap())[preset];
    if (presetConfig.asMap()) {
        // Set configuration (Config1 .. ConfigN)
        for (size_t i=0; i<m_configs.size(); i++) {
            std::string name = "Config" + Tools::number(i+1);
            if (presetConfig.asMap()->count(name) > 0) {
                reflectionzeug::AbstractProperty * prop = this->property(name);
                if (prop) {
                    MappingConfig * config = dynamic_cast<MappingConfig*>(prop);
                    if (config) {
                        copyConfig(config, (*presetConfig.asMap())[name]);
                    }
                }
            }
        }

        // Set classification
        if (presetConfig.asMap()->count("Classification") > 0 && this->propertyExists("Classification")) {
            this->setValue<std::string>("Classification", (*presetConfig.asMap())["Classification"].value<std::string>() );
        }

        // Update space-time-cube flag in all configurations
        for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
            (*it)->setPositionZ(m_spaceTimeCube ? -8 : -5);
        }
    }
}

std::string MappingConfigList::classification() const
{
    return m_classification;
}

void MappingConfigList::setClassification(const std::string & classification)
{
    m_classification = classification;
}

int MappingConfigList::classificationIndex() const
{
    if (m_classification == "Focus+Context") {
        return -1;
    } else {
        int index = Tools::indexOf(m_attributes, m_classification);
        if (index >= 0) {
            return index;
        } else {
            return 0;
        }
    }
}

bool MappingConfigList::spaceTimeCube() const
{
    return m_spaceTimeCube;
}

void MappingConfigList::setSpaceTimeCube(bool spaceTimeCube)
{
    m_spaceTimeCube = spaceTimeCube;

    // Update space-time-cube flag in all configurations
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        (*it)->setPositionZ(m_spaceTimeCube ? -8 : -5);
    }
}

float MappingConfigList::stcFactor() const
{
    return m_stcFactor;
}

void MappingConfigList::setStcFactor(float value)
{
    m_stcFactor = value;
}

bool MappingConfigList::checkUpdated()
{
    bool changed = false;
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        changed |= (*it)->checkUpdated();
    }
    return changed;
}

MappingConfig * MappingConfigList::getConfig(unsigned int index) const
{
    // Get property
    std::string name = "Config" + Tools::number((int)index+1);
    const reflectionzeug::AbstractProperty *prop = this->property(name);
    if (prop) {
        const MappingConfig * config = dynamic_cast<const MappingConfig*>(prop);
        if (config) {
            return (MappingConfig*)config;
        }
    }

    // Invalid index
    return nullptr;
}

unsigned int MappingConfigList::numConfigs() const
{
    return m_configs.size();
}

void MappingConfigList::setNumConfigs(unsigned int numConfigs)
{
    // Remove all configurations from property group
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        // [TODO] And then what? Delete it?
        MappingConfig * config = *it;
        takeProperty(config->name());
    }

    // Add or remove configurations
    if (m_configs.size() > numConfigs) {
        // Remove surplus configurations
        while (m_configs.size() > numConfigs) {
            MappingConfig * config = m_configs[m_configs.size()-1];
            if (config) {
                delete config;
            }
            m_configs.pop_back();
        }
    } else if (m_configs.size() < numConfigs) {
        // Add new configurations
        for (size_t i=m_configs.size(); i<numConfigs; i++) {
            MappingConfig * config = new MappingConfig(this, true, "Config" + Tools::number(i+1));
            m_configs.push_back(config);
        }
    }

    // Set title and add configurations to property group
    int index = 1;
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        MappingConfig * config = *it;
        std::string title = "Config #" + Tools::number(index);
        config->setOption("title", title);
        addProperty(config);
        index++;
    }
}

reflectionzeug::Variant MappingConfigList::stylePreset(const std::string & name) const
{
    reflectionzeug::Variant style;
    if (m_styles.asMap()->count(name) > 0) {
        style = (*m_styles.asMap()).at(name);
    }
    return style;
}

void MappingConfigList::setStylePreset(const std::string & name, const reflectionzeug::Variant & config)
{
    // Save style preset
    (*m_styles.asMap())[name] = config;

    // Get list of available styles
    std::vector<std::string> styles;
    for (auto it = m_styles.asMap()->begin(); it != m_styles.asMap()->end(); ++it) {
        styles.push_back(it->first);
    }

    // Update style-choices in properties
    for (size_t i=0; i<m_configs.size(); i++) {
        // Get configuration
        std::string name = "Config" + Tools::number(i+1);
        reflectionzeug::AbstractProperty * prop = this->property(name);
        MappingConfig * config = (prop ? dynamic_cast<MappingConfig*>(prop) : nullptr);
        if (config) {
            // Update property
            config->property("Style")->setOption("choices", styles);

            // Get LOD configurations
            for (int j=0; j<4; j++) {
                std::string name = "Lod" + Tools::number(j+1);
                reflectionzeug::AbstractProperty *prop = config->property(name);
                MappingConfig * lod = (prop ? dynamic_cast<MappingConfig*>(prop) : nullptr);
                if (lod) {
                    // Update property
                    lod->property("Style")->setOption("choices", styles);
                }
            }
        }
    }
}

reflectionzeug::Variant MappingConfigList::configPreset(const std::string &name) const
{
    return (*m_presets.asMap()).at(name);
}

void MappingConfigList::setConfigPreset(const std::string & name, const reflectionzeug::Variant & config)
{
    // Save preset
    (*m_presets.asMap())[name] = config;

    // Update preset-choices in properties
    std::vector<std::string> styles;
    for (auto it = m_presets.asMap()->begin(); it != m_presets.asMap()->end(); ++it) {
        styles.push_back(it->first);
    }
    PropertyGroup::property("Preset")->setOption("choices", styles);
}

const std::vector<std::string> & MappingConfigList::attributes() const
{
    return m_attributes;
}

void MappingConfigList::setAttributes(const std::vector<std::string> & attributes)
{
    m_attributes = attributes;

    std::vector<std::string> classifications = attributes;
    classifications.insert(classifications.begin(), "Focus+Context");
    PropertyGroup::property("Classification")->setOption("choices", classifications);

    for (auto it = m_configs.begin(); it != m_configs.end(); ++it)
    {
        MappingConfig * config = *it;
        config->setAttributes(attributes);        
    }
}

const std::vector<std::string> & MappingConfigList::colorMaps() const
{
    return m_colorMaps;
}

void MappingConfigList::setColorMaps(const std::vector<std::string> & colorMaps)
{
    m_colorMaps = colorMaps;
}

const std::vector<std::string> & MappingConfigList::textureMaps() const
{
    return m_textureMaps;
}

void MappingConfigList::setTextureMaps(const std::vector<std::string> & textureMaps)
{
    m_textureMaps = textureMaps;
}

void copyConfig(MappingConfig * config, const reflectionzeug::Variant & values)
{
    // Copy values
    if (values.asMap()->count("Style") > 0)          config->setValue<std::string> ("Style",          values.asMap()->at("Style")         .value<std::string>());
    if (values.asMap()->count("TextureID") > 0)      config->setValue<int>         ("TextureID",      values.asMap()->at("TextureID")     .value<int>());
    if (values.asMap()->count("ColorMapID") > 0)     config->setValue<int>         ("ColorMapID",     values.asMap()->at("ColorMapID")    .value<int>());
    if (values.asMap()->count("GeometryType") > 0)   config->setValue<int>         ("GeometryType",   values.asMap()->at("GeometryType")  .value<int>());
    if (values.asMap()->count("Tesselation") > 0)    config->setValue<int>         ("Tesselation",    values.asMap()->at("Tesselation")   .value<int>());
    if (values.asMap()->count("MinRadius") > 0)      config->setValue<float>       ("MinRadius",      values.asMap()->at("MinRadius")     .value<float>());
    if (values.asMap()->count("MaxRadius") > 0)      config->setValue<float>       ("MaxRadius",      values.asMap()->at("MaxRadius")     .value<float>());
    if (values.asMap()->count("StretchSize") > 0)    config->setValue<float>       ("StretchSize",    values.asMap()->at("StretchSize")   .value<float>());
    if (values.asMap()->count("TorsionSize") > 0)    config->setValue<float>       ("TorsionSize",    values.asMap()->at("TorsionSize")   .value<float>());
    if (values.asMap()->count("AnimationSpeed") > 0) config->setValue<float>       ("AnimationSpeed", values.asMap()->at("AnimationSpeed").value<float>());
    if (values.asMap()->count("PositionX") > 0)      config->setValue<MappingValue>("PositionX",      values.asMap()->at("PositionX")     .value<float>());
    if (values.asMap()->count("PositionY") > 0)      config->setValue<MappingValue>("PositionY",      values.asMap()->at("PositionY")     .value<float>());
    if (values.asMap()->count("PositionZ") > 0)      config->setValue<MappingValue>("PositionZ",      values.asMap()->at("PositionZ")     .value<float>());
    if (values.asMap()->count("Radius") > 0)         config->setValue<MappingValue>("Radius",         values.asMap()->at("Radius")        .value<float>());
    if (values.asMap()->count("Color") > 0)          config->setValue<MappingValue>("Color",          values.asMap()->at("Color")         .value<float>());
    if (values.asMap()->count("TextureX") > 0)       config->setValue<MappingValue>("TextureX",       values.asMap()->at("TextureX")      .value<float>());
    if (values.asMap()->count("TextureY") > 0)       config->setValue<MappingValue>("TextureY",       values.asMap()->at("TextureY")      .value<float>());
    if (values.asMap()->count("Alpha") > 0)          config->setValue<MappingValue>("Alpha",          values.asMap()->at("Alpha")         .value<float>());

    // Copy LODs
    for (int i=0; i<4; i++) {
        std::string name = "Lod" + Tools::number(i+1);
        if (values.asMap()->count(name) > 0) {
            reflectionzeug::AbstractProperty * prop = config->property(name);
            if (prop) {
                MappingConfig * config = dynamic_cast<MappingConfig*>(prop);
                if (config) {
                    copyConfig(config, values.asMap()->at(name));
                }
            }
        }
    }
}
