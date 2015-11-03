
#pragma once


#include <vector>

#include <glm/glm.hpp>


class Node;
class Line;


/**
*  @brief
*    Contains a data set of lines, edges, and/or nodes
*/
class DataSet
{
public:
    /**
    *  @brief
    *    Constructor
    */
    DataSet();

    /**
    *  @brief
    *    Destructor
    */
    ~DataSet();

    /**
    *  @brief
    *    Clear the data set
    */
    void clear();

    /**
    *  @brief
    *    Get nodes
    *
    *  @return
    *    List of nodes
    */
    const std::vector<Node *> & nodes() const;

    /**
    *  @brief
    *    Get lines
    *
    *  @return
    *    List of lines
    */
    const std::vector<Line *> & lines() const;

    /**
    *  @brief
    *    Add node
    *
    *  @param[in] node
    *    Node
    */
    void add(Node * node);

    /**
    *  @brief
    *    Add line
    *
    *  @param[in] line
    *    Line
    */
    void add(Line * line);


protected:
    std::vector<Node *> m_nodes;    ///< List of nodes
    std::vector<Line *> m_lines;    ///< List of lines
    uint32_t            m_nextId;   ///< Next free item ID
};
