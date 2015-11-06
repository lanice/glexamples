
#pragma once


#include <string>
#include <vector>

#include "attributemapping_api.h"


/**
*  @brief
*    Misc tools
*/
class ATTRIBUTEMAPPING_API Tools
{
public:
    /**
    *  @brief
    *    Convert number into string
    *
    *  @param[in] num
    *    Integral value
    *
    *  @return
    *    Number as string
    */
    static std::string number(size_t num);

    /**
    *  @brief
    *    Convert number into string
    *
    *  @param[in] num
    *    Integral value
    *
    *  @return
    *    Number as string
    */
    static std::string number(int num);

    /**
    *  @brief
    *    Convert number into string
    *
    *  @param[in] num
    *    Floating point value
    *
    *  @return
    *    Number as string
    */
    static std::string number(float num);

    /**
    *  @brief
    *    Get index of string in a string array
    *
    *  @param[in] array
    *    Array of strings
    *  @param[in] value
    *    String value
    *
    *  @return
    *    Index if string was found in the array, else -1
    */
    static int indexOf(const std::vector<std::string> & array, const std::string & value);
};
