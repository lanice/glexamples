
#include "NodeGeometry.h"

#include <vector>

#include <glm/glm.hpp>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/VertexAttributeBinding.h>

#include "DataSet.h"
#include "Node.h"
#include "Line.h"


using namespace globjects;


NodeGeometry::NodeGeometry()
: m_size(0)
{
}

NodeGeometry::~NodeGeometry()
{
}

void NodeGeometry::setData(const DataSet & dataSet)
{
    const std::vector<Node*> & nodes = dataSet.nodes();
    const std::vector<Line*> & lines = dataSet.lines();

    // Create vertex array
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> parameters;

    // If the database contains lines, add all nodes of all lines. Otherwise, add all nodes of the dataset.
    if (lines.size() > 0) {
        // Add all lines
        m_size = 0;
        for (std::vector<Line*>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            Line * line = *it;

            // Add all nodes
            for (std::vector<Node*>::const_iterator it2 = line->nodes().begin(); it2 != line->nodes().end(); ++it2) {
                Node * node = *it2;
                m_size++;

                // Position (lat / lon / height) in radians
                vertices.push_back(
                    glm::vec4(
                        node->position().x,
                        node->position().y,
                        node->position().z,
                        0.0f
                    )
                );

                // Node-ID / Line-ID / Time / -
                parameters.push_back(
                    glm::vec4(
                        (float)node->id(),
                        (float)line->id(),
                        -1.0f,
                        0.0f
                    )
                );
            }
        }
    } else {
        // Add nodes to vertex array
        m_size = nodes.size();
        for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
            Node * node = *it;

            // Position (lat / lon / height) in radians
            vertices.push_back(
                glm::vec4(
                    node->position().x,
                    node->position().y,
                    node->position().z,
                    0.0f
                )
            );

            // Node-ID / - / Time / -
            parameters.push_back(
                glm::vec4(
                    (float)node->id(),
                    0.0f,
                    -1.0f,
                    0.0f
                )
            );
        }

    }

    // Create VAO
    m_vao = new VertexArray;

    // Create binding for vertices
    {
        // Create buffer
        Buffer * buffer = new Buffer();
        buffer->setData(vertices, gl::GL_STATIC_DRAW);

        // Create binding
        auto binding = m_vao->binding(0);
        binding->setAttribute(0);
        binding->setBuffer(buffer, 0, sizeof(glm::vec4));
        binding->setFormat(4, gl::GL_FLOAT, gl::GL_FALSE, 0);
        m_vao->enable(0);
    }

    // Create binding for parameters
    {
        // Create buffer
        Buffer * buffer = new Buffer();
        buffer->setData(parameters, gl::GL_STATIC_DRAW);

        // Create binding
        auto binding = m_vao->binding(1);
        binding->setAttribute(1);
        binding->setBuffer(buffer, 0, sizeof(glm::vec4));
        binding->setFormat(4, gl::GL_FLOAT, gl::GL_FALSE, 0);
        m_vao->enable(1);
    }

    // Unbind VAO
    m_vao->unbind();
}

unsigned int NodeGeometry::size() const
{
    return m_size;
}

globjects::VertexArray * NodeGeometry::vao()
{
    return m_vao;
}

const globjects::VertexArray * NodeGeometry::vao() const
{
    return m_vao;
}

void NodeGeometry::draw()
{
    m_vao->drawArrays(gl::GL_POINTS, 0, m_size);
    m_vao->unbind();
}
