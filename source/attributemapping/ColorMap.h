
#pragma once


#include <string>


/**
*  @brief
*    Describes a color map (used for choosing color schemes via properties)
*/
class ColorMap
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] filename
    *    Filename
    */
    ColorMap(const std::string & filename);

    /**
    *  @brief
    *    Destructor
    */
    ~ColorMap();

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
