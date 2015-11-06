
#pragma once


#include <vector>
#include <string>

#include <reflectionzeug/property/AbstractTypedProperty.h>
#include <reflectionzeug/property/AbstractStringInterface.h>
#include <reflectionzeug/property/Property.h>

#include "ColorMap.h"
#include "TextureMap.h"
#include "MappingValue.h"


namespace reflectionzeug
{


class ATTRIBUTEMAPPING_API MappingValueProperty : public AbstractTypedProperty<MappingValue>
{
public:
    template <typename... Args>
    MappingValueProperty(Args&&... args);

    virtual ~MappingValueProperty();

    virtual std::string toString() const override;
    virtual bool fromString(const std::string & string) override;

    virtual reflectionzeug::Variant toVariant() const override;
    virtual bool fromVariant(const reflectionzeug::Variant & variant) override;

    std::vector<std::string> attributes() const;
    void setAttributes(const std::vector<std::string> & attributes);

    virtual void accept(AbstractVisitor * visitor) override;

protected:
    std::vector<std::string> m_attributes;
};

template <>
struct PropertyTypeSelector<MappingValue>
{
    using Type = MappingValueProperty;
};


class ATTRIBUTEMAPPING_API TextureProperty : public AbstractTypedProperty<TextureMap>
{
public:
    template <typename... Args>
    TextureProperty(Args&&... args);

    virtual ~TextureProperty();

    virtual std::string toString() const override;
    virtual bool fromString(const std::string & string) override;

    virtual reflectionzeug::Variant toVariant() const override;
    virtual bool fromVariant(const reflectionzeug::Variant & variant) override;

    std::vector<std::string> textures() const;
    void setTextures(const std::vector<std::string> & textures);

    virtual void accept(AbstractVisitor * visitor) override;

protected:
    std::vector<std::string> m_textures;
};

template <>
struct PropertyTypeSelector<TextureMap>
{
    using Type = TextureProperty;
};


class ATTRIBUTEMAPPING_API ColorMapProperty : public AbstractTypedProperty<ColorMap>
{
public:
    template <typename... Args>
    ColorMapProperty(Args&&... args);

    virtual ~ColorMapProperty();

    virtual std::string toString() const override;
    virtual bool fromString(const std::string & string) override;

    virtual reflectionzeug::Variant toVariant() const override;
    virtual bool fromVariant(const reflectionzeug::Variant & variant) override;

    virtual void accept(AbstractVisitor * visitor) override;
};

template <>
struct PropertyTypeSelector<ColorMap>
{
    using Type = ColorMapProperty;
};


} // namespace reflectionzeug


#include "PropertyExtensions.hpp"

