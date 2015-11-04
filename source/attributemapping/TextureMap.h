
#pragma once


#include <string>


/**
*  @brief
*    Describes a texture map (used for choosing a texture via properties)
*/
class TextureMap
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] filename
    *    Filename
    */
    TextureMap(const std::string & filename);

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
