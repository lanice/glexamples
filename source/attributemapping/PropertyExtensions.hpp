
#pragma once


#include "PropertyExtensions.h"


namespace reflectionzeug
{


template <typename... Args>
MappingValueProperty::MappingValueProperty(Args&&... args)
: AbstractTypedProperty<MappingValue>(std::forward<Args>(args)...)
{
    setOption("minimum", -128.0f);
}

template <typename... Args>
TextureProperty::TextureProperty(Args&&... args)
: AbstractTypedProperty<TextureMap>(std::forward<Args>(args)...)
{
}

template <typename... Args>
ColorMapProperty::ColorMapProperty(Args&&... args)
: AbstractTypedProperty<ColorMap>(std::forward<Args>(args)...)
{
}


} // namespace reflectionzeug
