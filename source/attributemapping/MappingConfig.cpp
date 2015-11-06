
#include "MappingConfig.h"

#include "MappingConfigList.h"
#include "PropertyExtensions.h"
#include "Tools.h"


MappingConfig::MappingConfig(MappingConfigList * configList, bool useLod, std::string name)
: reflectionzeug::Object(name)
, m_configList(configList)
, m_useLod(useLod)
, m_updated(true)
, m_style("Default")
, m_textureID(1)
, m_texture(configList->textureMaps()[0])
, m_colorMapID(19)
, m_colorMap(configList->colorMaps()[0])
, m_geometryType(1)
, m_tesselation(6)
, m_minRadius(100.0f)
, m_maxRadius(400.0f)
, m_stretchSize(8.0f)
, m_torsionSize(8.0f)
, m_animationSpeed(0.7f)
, m_upVector(0.0f, 0.0f, 1.0f)
, m_positionX((float)-3)
, m_positionY((float)-4)
, m_positionZ((float)-5)
, m_radius(0.0f)
, m_color((float)-10)
, m_textureX((float)-6)
, m_textureY(0.0f)
, m_alpha(1.0f)
{
    // Register config properties
    addProperty<std::string>("Style", this, &MappingConfig::getStyle, &MappingConfig::setStyle);
    std::vector<std::string> choices = { "Default" };
    PropertyGroup::property("Style")->setOption("choices", choices);

    addProperty<int>("TextureID", this, &MappingConfig::textureID, &MappingConfig::setTextureID);

    auto * propTexture = addProperty<TextureMap>("Texture", this, &MappingConfig::texture, &MappingConfig::setTexture);
    propTexture->setTextures(m_configList->textureMaps());

    addProperty<int>("ColorMapID", this, &MappingConfig::colorMapID, &MappingConfig::setColorMapID);

    addProperty<std::string>("ColorMap", this, &MappingConfig::colorMap, &MappingConfig::setColorMap);
    PropertyGroup::property("ColorMap")->setOption("title", "Color map");

    addProperty<int>("GeometryType", this, &MappingConfig::geometryType, &MappingConfig::setGeometryType);

    addProperty<int>("Tesselation", this, &MappingConfig::tesselation, &MappingConfig::setTesselation);
    PropertyGroup::property("Tesselation")->setOption("minimum",  0);
    PropertyGroup::property("Tesselation")->setOption("maximum", 64);

    addProperty<float>("MinRadius", this, &MappingConfig::minRadius, &MappingConfig::setMinRadius);

    addProperty<float>("MaxRadius", this, &MappingConfig::maxRadius, &MappingConfig::setMaxRadius);

    addProperty<float>("StretchSize", this, &MappingConfig::stretchSize, &MappingConfig::setStretchSize);

    addProperty<float>("TorsionSize", this, &MappingConfig::torsionSize, &MappingConfig::setTorsionSize);

    addProperty<float>("AnimationSpeed", this, &MappingConfig::animationSpeed, &MappingConfig::setAnimationSpeed);
    PropertyGroup::property("AnimationSpeed")->setOption("minimum",   0.0f);
    PropertyGroup::property("AnimationSpeed")->setOption("maximum", 100.0f);

//  addProperty<glm::vec3>("UpVector", this, &MappingConfig::upVector, &MappingConfig::setUpVector);

    addProperty<MappingValue>("PositionX", this, &MappingConfig::positionX, &MappingConfig::setPositionX);

    addProperty<MappingValue>("PositionY", this, &MappingConfig::positionY, &MappingConfig::setPositionY);

    addProperty<MappingValue>("PositionZ", this, &MappingConfig::positionZ, &MappingConfig::setPositionZ);

    addProperty<MappingValue>("Radius", this, &MappingConfig::radius, &MappingConfig::setRadius);

    addProperty<MappingValue>("Color", this, &MappingConfig::color, &MappingConfig::setColor);

    addProperty<MappingValue>("TextureX", this, &MappingConfig::textureX, &MappingConfig::setTextureX);

    addProperty<MappingValue>("TextureY", this, &MappingConfig::textureY, &MappingConfig::setTextureY);

    addProperty<MappingValue>("Alpha", this, &MappingConfig::alpha, &MappingConfig::setAlpha);

    // Create LOD configurations
    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            // Create LOD config
            MappingConfig * lod = new MappingConfig(m_configList, false, "Lod"  + Tools::number(i));
            lod->setOption( "title", (i==4) ? "Highlight" : ("Lod " + Tools::number(i)) );

            // Add to property group
            addProperty(lod);
        }
    }
}

MappingConfig::~MappingConfig()
{
}

void MappingConfig::setAttributes(const std::vector<std::string> & attributes)
{
    m_attributes = attributes;

    if (m_useLod) {
        getLodConfig(1)->setAttributes(attributes);
        getLodConfig(2)->setAttributes(attributes);
        getLodConfig(3)->setAttributes(attributes);
        getLodConfig(4)->setAttributes(attributes);
    }

    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("PositionX"))->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("PositionY"))->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("PositionZ"))->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("Radius"))->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("Color")) ->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("TextureX"))->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("TextureY"))->setAttributes(m_attributes);
    dynamic_cast<reflectionzeug::MappingValueProperty*>(PropertyGroup::property("Alpha"))->setAttributes(m_attributes);
}

bool MappingConfig::checkUpdated()
{
    // Check if this config or sub-configs have been updated
    bool updated = m_updated;
    if (m_useLod) {
        updated |= getLodConfig(1)->checkUpdated();
        updated |= getLodConfig(2)->checkUpdated();
        updated |= getLodConfig(3)->checkUpdated();
        updated |= getLodConfig(4)->checkUpdated();
    }

    // Reset flag
    m_updated = false;

    // Return result
    return updated;
}

std::string MappingConfig::getStyle() const
{
    return m_style;
}

void MappingConfig::setStyle(const std::string & style)
{
    // Get configuration for the selected style and check if it is valid
    reflectionzeug::Variant config = m_configList->stylePreset(style);
    reflectionzeug::VariantMap * map = config.asMap();
    if (map) {
        // Save style name
        m_style = style;

        // Apply configuration
        if (map->count("TextureID") > 0)      setValue<int>  ("TextureID",        map->at("TextureID")     .value<int>());
        if (map->count("ColorMapID") > 0)     setValue<int>  ("ColorMapID",       map->at("ColorMapID")    .value<int>());
        if (map->count("GeometryType") > 0)   setValue<int>  ("GeometryType",     map->at("GeometryType")  .value<int>());
        if (map->count("Tesselation") > 0)    setValue<int>  ("Tesselation",      map->at("Tesselation")   .value<int>());
        if (map->count("MinRadius") > 0)      setValue<float>("MinRadius",        map->at("MinRadius")     .value<float>());
        if (map->count("MaxRadius") > 0)      setValue<float>("MaxRadius",        map->at("MaxRadius")     .value<float>());
        if (map->count("StretchSize") > 0)    setValue<float>("StretchSize",      map->at("StretchSize")   .value<float>());
        if (map->count("TorsionSize") > 0)    setValue<float>("TorsionSize",      map->at("TorsionSize")   .value<float>());
        if (map->count("AnimationSpeed") > 0) setValue<float>("AnimationSpeed",   map->at("AnimationSpeed").value<float>());
        if (map->count("PositionX") > 0)      setValue<MappingValue>("PositionX", map->at("PositionX")     .value<float>());
        if (map->count("PositionY") > 0)      setValue<MappingValue>("PositionY", map->at("PositionY")     .value<float>());
        if (map->count("PositionZ") > 0)      setValue<MappingValue>("PositionZ", map->at("PositionZ")     .value<float>());
        if (map->count("Radius") > 0)         setValue<MappingValue>("Radius",    map->at("Radius")        .value<float>());
        if (map->count("Color") > 0)          setValue<MappingValue>("Color",     map->at("Color")         .value<float>());
        if (map->count("TextureX") > 0)       setValue<MappingValue>("TextureX",  map->at("TextureX")      .value<float>());
        if (map->count("TextureY") > 0)       setValue<MappingValue>("TextureY",  map->at("TextureY")      .value<float>());
        if (map->count("Alpha") > 0)          setValue<MappingValue>("Alpha",     map->at("Alpha")         .value<float>());
    }
}

MappingConfig * MappingConfig::getLodConfig(unsigned int lod) const
{
    // Check if LOD is activated in the level is between 1 and 4
    if (m_useLod && lod >= 1 && lod <= 4) {
        // Get property
        std::string name = "Lod" + Tools::number((int)lod);
        const reflectionzeug::AbstractProperty *prop = this->property(name);
        if (prop) {
            const MappingConfig * config = dynamic_cast<const MappingConfig*>(prop);
            if (config) {
                return (MappingConfig*)config;
            }
        }
    }

    // Invalid index or LOD deactivated
    return (MappingConfig*)this;
}

int MappingConfig::textureID() const
{
    return m_textureID;
}

void MappingConfig::setTextureID(int id)
{
    m_textureID = id;
    m_updated   = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".TextureID", id);
        }
    }

    // Update texture string
    m_texture = m_configList->textureMaps()[id];
}

TextureMap MappingConfig::texture() const
{
    return m_texture;
}

void MappingConfig::setTexture(const TextureMap & texture)
{
    // Set texture
    m_texture = texture;

    // Update chosen texture index
    int index = Tools::indexOf(m_configList->textureMaps(), texture.filename());
    setTextureID(index);
}

int MappingConfig::colorMapID() const
{
    return m_colorMapID;
}

void MappingConfig::setColorMapID(int id)
{
    m_colorMapID = id;
    m_updated    = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".ColorMapID", id);
        }
    }

    // Update color map string
    m_colorMap = m_configList->colorMaps()[id];
}

std::string MappingConfig::colorMap() const
{
    // Return color map
    return m_colorMap;
}

void MappingConfig::setColorMap(const std::string & colorMap)
{
    // Set color map
    m_colorMap = colorMap;

    // Update chosen color map index
    int index = Tools::indexOf(m_configList->colorMaps(), colorMap);
    setColorMapID(index);
}

int MappingConfig::geometryType() const
{
    return m_geometryType;
}

void MappingConfig::setGeometryType(int type)
{
    m_geometryType = type;
    m_updated      = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".GeometryType", type);
        }
    }
}

int MappingConfig::tesselation() const
{
    return m_tesselation;
}

void MappingConfig::setTesselation(int tesselation)
{
    m_tesselation = tesselation;
    m_updated     = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".Tesselation", tesselation);
        }
    }
}

float MappingConfig::minRadius() const
{
    return m_minRadius;
}

void MappingConfig::setMinRadius(float radius)
{
    m_minRadius = radius;
    m_updated   = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".MinRadius", radius);
        }
    }
}

float MappingConfig::maxRadius() const
{
    return m_maxRadius;
}

void MappingConfig::setMaxRadius(float radius)
{
    m_maxRadius = radius;
    m_updated   = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".MaxRadius", radius);
        }
    }
}

float MappingConfig::stretchSize() const
{
    return m_stretchSize;
}

void MappingConfig::setStretchSize(float size)
{
    m_stretchSize = size;
    m_updated     = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".StretchSize", size);
        }
    }
}

float MappingConfig::torsionSize() const
{
    return m_torsionSize;
}

void MappingConfig::setTorsionSize(float size)
{
    m_torsionSize = size;
    m_updated     = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".TorsionSize", size);
        }
    }
}

float MappingConfig::animationSpeed() const
{
    return m_animationSpeed;
}

void MappingConfig::setAnimationSpeed(float speed)
{
    m_animationSpeed = speed;
    m_updated        = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".AnimationSpeed", speed);
        }
    }
}

glm::vec3 MappingConfig::upVector() const
{
    return m_upVector;
}

void MappingConfig::setUpVector(const glm::vec3 & up)
{
    m_upVector = up;
    m_updated  = true;

    /*
    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".UpVector", up);
        }
    }
    */
}

MappingValue MappingConfig::positionX() const
{
    return m_positionX;
}

void MappingConfig::setPositionX(MappingValue x)
{
    m_positionX = x;
    m_updated   = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".PositionX", x);
        }
    }
}

MappingValue MappingConfig::positionY() const
{
    return m_positionY;
}

void MappingConfig::setPositionY(MappingValue y)
{
    m_positionY = y;
    m_updated   = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".PositionY", y);
        }
    }
}

MappingValue MappingConfig::positionZ() const
{
    return m_positionZ;
}

void MappingConfig::setPositionZ(MappingValue z)
{
    m_positionZ = z;
    m_updated   = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".PositionZ", z);
        }
    }
}

MappingValue MappingConfig::radius() const
{
    return m_radius;
}

void MappingConfig::setRadius(MappingValue radius)
{
    m_radius  = radius;
    m_updated = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".Radius", radius);
        }
    }
}

MappingValue MappingConfig::color() const
{
    return m_color;
}

void MappingConfig::setColor(MappingValue color)
{
    m_color   = color;
    m_updated = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".Color", color);
        }
    }
}

MappingValue MappingConfig::textureX() const
{
    return m_textureX;
}

void MappingConfig::setTextureX(MappingValue textureX)
{
    m_textureX = textureX;
    m_updated  = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".TextureX", textureX);
        }
    }
}

MappingValue MappingConfig::textureY() const
{
    return m_textureY;
}

void MappingConfig::setTextureY(MappingValue textureY)
{
    m_textureY = textureY;
    m_updated  = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".TextureY", textureY);
        }
    }
}

MappingValue MappingConfig::alpha() const
{
    return m_alpha;
}

void MappingConfig::setAlpha(MappingValue alpha)
{
    m_alpha   = alpha;
    m_updated = true;

    if (m_useLod) {
        for (int i=1; i<=4; i++) {
            this->setValue("Lod" + Tools::number(i) + ".Alpha", alpha);
        }
    }
}
