
#include "MappingValue.h"


MappingValue::MappingValue()
: m_value(0.0f)
{
}

MappingValue::MappingValue(float value)
: m_value(value)
{
}

MappingValue::MappingValue(const MappingValue & source)
: m_value(source.m_value)
{
}

MappingValue::~MappingValue()
{
}

const MappingValue & MappingValue::operator =(const MappingValue & source)
{
    m_value = source.m_value;
    return *this;
}

float MappingValue::value() const
{
    return m_value;
}

void MappingValue::setValue(float value)
{
    m_value = value;
}
