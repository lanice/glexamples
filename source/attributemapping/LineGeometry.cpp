
#include "LineGeometry.h"

#include <vector>

#include <glm/glm.hpp>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/VertexAttributeBinding.h>

#include "DataSet.h"
#include "Node.h"
#include "Line.h"


using namespace globjects;


LineGeometry::LineGeometry()
: m_size(0)
{
}

LineGeometry::~LineGeometry()
{
}

void LineGeometry::setData(const DataSet & dataset)
{
    // Create vertex array
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> parameters;

    // Iterate over lines
    m_size = 0;
    const std::vector<Line*> & lines = dataset.lines();
    for (std::vector<Line*>::const_iterator it = lines.begin(); it != lines.end(); ++it)
    {
        Line * line = *it;

        // Add nodes to vertex array
        bool first = true;
        for (std::vector<Node*>::const_iterator it2 = line->nodes().begin(); it2 != line->nodes().end(); ++it2)
        {
            Node * node = *it2;

            // [DEBUG]
            // std::cout << "(" << node->position().getLat() << ", " << node->position().getLong() << ", " << node->position().getHeight() << ")\n";

            // Duplicate first and last node
            int count = (first || it2 == line->nodes().end()) ? 2 : 1;
            for (int i=0; i<count; i++)
            {
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

                m_size++;
            }
        
            first = false;
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

unsigned int LineGeometry::size() const
{
    return m_size;
}

globjects::VertexArray * LineGeometry::vao()
{
    return m_vao;
}

const globjects::VertexArray * LineGeometry::vao() const
{
    return m_vao;
}

void LineGeometry::draw()
{
    m_vao->drawArrays(gl::GL_LINE_STRIP_ADJACENCY, 0, m_size);
    m_vao->unbind();
}
