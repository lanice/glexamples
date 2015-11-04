
#include "TextureMap.h"


TextureMap::TextureMap(const std::string & filename)
: m_filename(filename)
{
}

TextureMap::~TextureMap()
{
}

std::string TextureMap::filename() const
{
    return m_filename;
}
