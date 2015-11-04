
#include "DataSet.h"

#include "Node.h"
#include "Line.h"


DataSet::DataSet()
: m_nextId(1)
{
}

DataSet::~DataSet()
{
    // Clear data
    clear();
}

void DataSet::clear()
{
    // Delete lines
    for (size_t i=0; i<m_lines.size(); i++) {
        delete m_lines[i];
    }
    m_lines.clear();

    // Delete nodes
    for (size_t i=0; i<m_nodes.size(); i++) {
        delete m_nodes[i];
    }
    m_nodes.clear();
}

const std::vector<Node *> & DataSet::nodes() const
{
    return m_nodes;
}

const std::vector<Line *> & DataSet::lines() const
{
    return m_lines;
}

void DataSet::add(Node * node)
{
    node->setId(m_nextId++);
    m_nodes.push_back(node);
}

void DataSet::add(Line * line)
{
    line->setId(m_nextId++);
    m_lines.push_back(line);
}

const std::vector<std::string> & DataSet::nodeAttributes() const
{
    return m_nodeAttributes;
}

void DataSet::setNodeAttributes(const std::vector<std::string> & attributes)
{
    m_nodeAttributes = attributes;
}

const std::vector<std::string> & DataSet::lineAttributes() const
{
    return m_lineAttributes;
}

void DataSet::setLineAttributes(const std::vector<std::string> & attributes)
{
    m_lineAttributes = attributes;
}
