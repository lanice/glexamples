
#pragma once


#include <vector>

#include "DataItem.h"


class Node;


/**
*  @brief
*    Describes a line (list of connected nodes)
*/
class ATTRIBUTEMAPPING_API Line : public DataItem
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] id
    *    ID
    */
    Line(uint32_t id = 0);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~Line();

    /**
    *  @brief
    *    Clear all data
    */
    void clear();

    /**
    *  @brief
    *    Check if line is empty
    *
    *  @return
    *    'true' if line is empty, else 'false'
    */
    bool isEmpty() const;

    /**
    *  @brief
    *    Get nodes
    *
    *  @return
    *    Node list
    */
    const std::vector<Node*> & nodes() const;

    /**
    *  @brief
    *    Add node
    *
    *  @param[in] node
    *    Node
    */
    void add(Node * node);


protected:
    std::vector<Node*> m_nodes; ///< List of nodes
};
