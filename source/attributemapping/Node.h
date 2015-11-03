
#pragma once


#include <glm/glm.hpp>

#include "DataItem.h"


/**
*  @brief
*    Describes a node or sample point
*/
class Node : public DataItem
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] id
    *    Node ID
    */
    Node(uint32_t id = 0);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~Node();

    /**
    *  @brief
    *    Get position
    *
    *  @return
    *    Position
    */
    glm::vec3 position() const;

    /**
    *  @brief
    *    Set position
    *
    *  @param[in] pos
    *    Position
    */
    void setPosition(const glm::vec3 & pos);


protected:
    glm::vec3 m_pos;    ///< Node position
};
