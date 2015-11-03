
#include "Line.h"


Line::Line(uint32_t id)
: DataItem(id)
{
}

Line::~Line()
{
}

void Line::clear()
{
    // Clear lists (will be only emptied, but the elements will not really be destroyed).
    m_nodes.clear();
}

bool Line::isEmpty() const
{
    return m_nodes.empty();
}

const std::vector<Node*> & Line::nodes() const
{
    return m_nodes;
}

void Line::add(Node * node)
{
    m_nodes.push_back(node);
}
