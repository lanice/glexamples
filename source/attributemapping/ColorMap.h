
#pragma once


#include <string>

#include "attributemapping_api.h"


/**
*  @brief
*    Describes a color map (used for choosing color schemes via properties)
*/
class ATTRIBUTEMAPPING_API ColorMap
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] filename
    *    Filename
    */
    ColorMap(const std::string & filename = "");

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
