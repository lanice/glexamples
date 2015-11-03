
#include "Node.h"


Node::Node(uint32_t id)
: DataItem(id)
{
}

Node::~Node()
{
}

glm::vec3 Node::position() const
{
    return m_pos;
}

void Node::setPosition(const glm::vec3 & pos)
{
    m_pos = pos;
}
