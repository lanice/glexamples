
#pragma once


#include "attributemapping_api.h"


/**
*  @brief
*    Attribute value describing either a static value (>= 0.0), or mapping a property value (< 0.0)
*
*  @remarks
*    If value is >= 0.0, (value) is used as the actual static attribute value.
*    If value is <  0.0, (value * (-1) - 1) is the index of the property that is mapped to the actual attribute value.
*/
class ATTRIBUTEMAPPING_API MappingValue
{
public:
    MappingValue();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] value
    *    Value
    */
    MappingValue(float value);

    /**
    *  @brief
    *    Copy constructor
    *
    *  @param source
    *    Source vaue
    */
    MappingValue(const MappingValue & source);

    /**
    *  @brief
    *    Destructor
    */
    ~MappingValue();

    /**
    *  @brief
    *    Copy operator
    *
    *  @param source
    *    Source value
    */
    const MappingValue & operator =(const MappingValue & source);

    /**
    *  @brief
    *    Get value
    *
    *  @return
    *    Value
    */
    float value() const;

    /**
    *  @brief
    *    Set value
    *
    *  @param[in] value
    *    Value
    */
    void setValue(float value);


protected:
    float m_value; ///< Value
};
