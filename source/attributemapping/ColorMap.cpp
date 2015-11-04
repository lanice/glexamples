
#include "ColorMap.h"


ColorMap::ColorMap(const std::string & filename)
: m_filename(filename)
{
}

ColorMap::~ColorMap()
{
}

std::string ColorMap::filename() const
{
    return m_filename;
}
