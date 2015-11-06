
#include "PropertyExtensions.h"

#include <string>

#include <reflectionzeug/property/AbstractVisitor.h>

#include "Tools.h"


namespace reflectionzeug
{


MappingValueProperty::~MappingValueProperty()
{
}

std::string MappingValueProperty::toString() const
{
    std::string valueString;

    // Check if value is set directly or is mapping to an attribute
    float value = this->value().value();
    if (value < 0.0f) {
        // Get attribute
        int index = ((int)(-value) - 1);
        if (index >= 0 && index < (int)m_attributes.size()) {
            // Return name of attribute that is mapped
            valueString = m_attributes[index];
        }
    } else {
        // Return direct value as a string
        valueString = Tools::number(value);
    }

    return valueString;
}

bool MappingValueProperty::fromString(const std::string & string)
{
    // Check if value is a attribute name or a direct value
    int index = Tools::indexOf(m_attributes, string);
    if (index >= 0 && index < (int)m_attributes.size()) {
        // Set index of mapped attribute
        this->setValue((float)(-index - 1));
    } else {
        // Set direct value
        float value = std::stod(string);
        this->setValue(value);
    }

    return true;
}

reflectionzeug::Variant MappingValueProperty::toVariant() const
{
    return reflectionzeug::Variant(toString());
}

bool MappingValueProperty::fromVariant(const reflectionzeug::Variant & variant)
{
    return this->fromString(variant.value<std::string>());
}

void MappingValueProperty::accept(AbstractVisitor * visitor)
{
    visitor->callVisitor<MappingValueProperty>(this);
}

std::vector<std::string> MappingValueProperty::attributes() const
{
    return m_attributes;
}

void MappingValueProperty::setAttributes(const std::vector<std::string> & attributes)
{
    m_attributes = attributes;
}


TextureProperty::~TextureProperty()
{
}

std::string TextureProperty::toString() const
{
    return this->value().filename();
}

bool TextureProperty::fromString(const std::string & string)
{
    this->setValue(TextureMap(string));
    return true;
}

reflectionzeug::Variant TextureProperty::toVariant() const
{
    return reflectionzeug::Variant(toString());
}

bool TextureProperty::fromVariant(const reflectionzeug::Variant & variant)
{
    return this->fromString(variant.value<std::string>());
}

void TextureProperty::accept(AbstractVisitor * visitor)
{
    visitor->callVisitor<TextureProperty>(this);
}

std::vector<std::string> TextureProperty::textures() const
{
    return m_textures;
}

void TextureProperty::setTextures(const std::vector<std::string> & textures)
{
    m_textures = textures;
}


ColorMapProperty::~ColorMapProperty()
{
}

std::string ColorMapProperty::toString() const
{
    return this->value().filename();
}

bool ColorMapProperty::fromString(const std::string & string)
{
    this->setValue(ColorMap(string));
    return true;
}

reflectionzeug::Variant ColorMapProperty::toVariant() const
{
    return reflectionzeug::Variant(toString());
}

bool ColorMapProperty::fromVariant(const reflectionzeug::Variant & variant)
{
    return this->fromString(variant.value<std::string>());
}

void ColorMapProperty::accept(AbstractVisitor * visitor)
{
    visitor->callVisitor<ColorMapProperty>(this);
}


} // namespace reflectionzeug
