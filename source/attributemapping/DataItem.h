
#pragma once


#include <glm/glm.hpp>

#include "attributemapping_api.h"


/**
*  @brief
*    Base class for data items such as nodes, edges, or lines
*/
class ATTRIBUTEMAPPING_API DataItem
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] id
    *    Item ID
    */
    DataItem(uint32_t id = 0);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~DataItem();

    /**
    *  @brief
    *    Get ID
    *
    *  @return
    *    Item ID
    */
    uint32_t id() const;

    /**
    *  @brief
    *    Set ID
    *
    *  @param id
    *    Item ID
    */
    void setId(uint32_t id);

    /**
    *  @brief
    *    Get attribute value
    *
    *  @param[in] index
    *    Attribute index
    *
    *  @return
    *    Attribute value
    */
    float attribute(size_t index) const;

    /**
    *  @brief
    *    Set attribute value
    *
    *  @param[in] index
    *    Attribute index
    *  @param[in] value
    *    Attribute value
    */
    void setAttribute(size_t index, float value);


protected:
    uint32_t m_id;          ///< Item ID
    float    m_attrs[16];   ///< Item attributes
};
