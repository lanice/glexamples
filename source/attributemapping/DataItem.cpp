
#include "DataItem.h"


DataItem::DataItem(uint32_t id)
: m_id(id)
{
}

DataItem::~DataItem()
{
}

float DataItem::attribute(size_t index) const
{
    // Check index
    if (index >= 16) {
        return 0.0f;
    }

    // Set attribute
    return m_attrs[index];
}

void DataItem::setAttribute(size_t index, float value)
{
    // Check index
    if (index >= 16) {
        return;
    }

    // Set attribute
    m_attrs[index] = value;
}

uint32_t DataItem::id() const
{
    return m_id;
}
