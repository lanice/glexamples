
#include "PropertyExtensions.h"

#include <string>

#include <reflectionzeug/property/AbstractVisitor.h>


namespace reflectionzeug
{


MappingValueProperty::~MappingValueProperty()
{
}

std::string MappingValueProperty::toString() const
{
    std::string valueString;
    float value = this->value().value();
    if (value < 0.0f) {
        int index = ((int)(-value) - 1);
        if (index >= 0 && index < m_attributes.length()) {
            valueString = m_attributes[index];
        }
    } else {
        std::stringstream stream;
        stream << value;
        valueString = stream.str();
    }

    return valueString.toStdString();
}

bool MappingValueProperty::fromString(const std::string & string)
{
    int index = m_attributes.indexOf(string);
    if (index >= 0 && index < m_attributes.length()) {
        this->setValue((float)(-index - 1));
    } else {
        this->setValue(string.toFloat());
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
    return this->value().filename().toStdString();
}

bool TextureProperty::fromString(const std::string & string)
{
    this->setValue(gtx::TextureMap(QString::fromStdString(string)));
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
    return this->value().toString().toStdString();
}

bool ColorMapProperty::fromString(const std::string & string)
{
    this->setValue(QString::fromStdString(string));
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
