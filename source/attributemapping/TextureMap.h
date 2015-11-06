
#pragma once


#include <string>

#include "attributemapping_api.h"


/**
*  @brief
*    Describes a texture map (used for choosing textures via properties)
*/
class ATTRIBUTEMAPPING_API TextureMap
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] filename
    *    Filename
    */
    TextureMap(const std::string & filename = "");

    /**
    *  @brief
    *    Destructor
    */
    ~TextureMap();

    /**
    *  @brief
    *    Get filename
    *
    *  @return
    *    Filename
    */
    std::string filename() const;


protected:
    std::string m_filename; ///< Texture filename
};
