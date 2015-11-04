
#include "AttributeStorage.h"

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>

#include "DataSet.h"
#include "Node.h"
#include "Line.h"


using namespace globjects;


AttributeStorage::AttributeStorage()
: m_size(0)
, m_numNodeAttributes(0)
{
}

AttributeStorage::~AttributeStorage()
{
}

void AttributeStorage::setData(const DataSet & dataset)
{
    static const int attrMin  = 0;  // Minimum value
    static const int attrMax  = 1;  // Maximum value
    static const int attrMean = 2;  // Mean value
    static const int attrDev  = 3;  // Standard deviation

    // Set maximum number of attributes
    int numAttrs = 32;

    // Get number of items
    int numItems  = dataset.nodes().size() + dataset.lines().size();
    int numChunks = numItems + 4;   // 0-4: minValues, maxValues, reserved, reserved

    // Determine texture size
    int numFloats = numChunks * numAttrs;
    numFloats = ((numFloats / 1024) + 1) * 1024;

    // Create texture data
    float * data = (float*)malloc(numFloats * sizeof(float));
    for (int i=0; i<numFloats; i++) {
        data[i] = 0.0f;
    }

    // Clear attributes
    m_attrs.clear();
    int attrIndex = 0;

    // Add static attributes
    int numStaticAttrs = 0;
    {
        m_attrs.push_back("Zero");
        m_attrs.push_back("One");
        m_attrs.push_back("PosX");
        m_attrs.push_back("PosY");
        m_attrs.push_back("PosZ");
        m_attrs.push_back("nodeId");
        m_attrs.push_back("lineId");
        numStaticAttrs = m_attrs.size();

        // Zero
        data[attrMin  * numAttrs + 0] = 0.0;    // Minimum
        data[attrMax  * numAttrs + 0] = 0.0;    // Maximum
        data[attrMean * numAttrs + 0] = 0.0;    // Sum (later: Mean)

        // One
        data[attrMin  * numAttrs + 1] = 1.0;    // Minimum
        data[attrMax  * numAttrs + 1] = 1.0;    // Maximum
        data[attrMean * numAttrs + 1] = 1.0;    // Sum (later: Mean)

        // Lat
        data[attrMin  * numAttrs + 2] = 0.0;    // Minimum
        data[attrMax  * numAttrs + 2] = 0.0;    // Maximum
        data[attrMean * numAttrs + 2] = 0.0;    // Sum (later: Mean)

        // Long
        data[attrMin  * numAttrs + 3] = 0.0;    // Minimum
        data[attrMax  * numAttrs + 3] = 0.0;    // Maximum
        data[attrMean * numAttrs + 3] = 0.0;    // Sum (later: Mean)

        // Height
        data[attrMin  * numAttrs + 4] = 0.0;    // Minimum
        data[attrMax  * numAttrs + 4] = 0.0;    // Maximum
        data[attrMean * numAttrs + 4] = 0.0;    // Sum (later: Mean)

        // nodeId
        data[attrMin  * numAttrs + 5] = 0.0;    // Minimum
        data[attrMax  * numAttrs + 5] = 0.0;    // Maximum
        data[attrMean * numAttrs + 5] = 0.0;    // Sum (later: Mean)

        // lineId
        data[attrMin  * numAttrs + 6] = -1.0;    // Minimum
        data[attrMax  * numAttrs + 6] = -1.0;    // Maximum
        data[attrMean * numAttrs + 6] = -1.0;    // Sum (later: Mean)

        for (Node * node : dataset.nodes())
        {
            int nodeIndex = node->id();

            // Zero
            {
                int index = (nodeIndex + 4) * numAttrs + 0;
                data[index] = 0.0f;
                data[attrMin  * numAttrs + attrIndex + 0] = 0.0f;
                data[attrMax  * numAttrs + attrIndex + 0] = 1.0f;
                data[attrMean * numAttrs + attrIndex + 0] = 0.5f;
            }

            // One
            {
                int index = (nodeIndex + 4) * numAttrs + 1;
                data[index] = 1.0f;
                data[attrMin  * numAttrs + attrIndex + 1] = 0.0f;
                data[attrMax  * numAttrs + attrIndex + 1] = 1.0f;
                data[attrMean * numAttrs + attrIndex + 1] = 0.5f;
            }

            // PosX
            {
                int index = (nodeIndex + 4) * numAttrs + 2;
                float value = node->position().x;
                data[index] = value;

                if (nodeIndex == 0) {
                    data[attrMin  * numAttrs + attrIndex + 2] = value;
                    data[attrMax  * numAttrs + attrIndex + 2] = value;
                    data[attrMean * numAttrs + attrIndex + 2] = value;
                } else {
                    data[attrMin  * numAttrs + attrIndex + 2]  = glm::min(value, data[attrMin  * numAttrs + attrIndex + 2]);
                    data[attrMax  * numAttrs + attrIndex + 2]  = glm::max(value, data[attrMax  * numAttrs + attrIndex + 2]);
                    data[attrMean * numAttrs + attrIndex + 2] += value;
                }
            }

            // PosY
            {
                int index = (nodeIndex + 4) * numAttrs + 3;
                float value = node->position().y;
                data[index] = value;

                if (nodeIndex == 0) {
                    data[attrMin  * numAttrs + attrIndex + 3] = value;
                    data[attrMax  * numAttrs + attrIndex + 3] = value;
                    data[attrMean * numAttrs + attrIndex + 3] = value;
                } else {
                    data[attrMin  * numAttrs + attrIndex + 3]  = glm::min(value, data[attrMin  * numAttrs + attrIndex + 3]);
                    data[attrMax  * numAttrs + attrIndex + 3]  = glm::max(value, data[attrMax  * numAttrs + attrIndex + 3]);
                    data[attrMean * numAttrs + attrIndex + 3] += value;
                }
            }

            // PosZ
            {
                int index = (nodeIndex + 4) * numAttrs + 4;
                float value = node->position().z;
                data[index] = value;

                if (nodeIndex == 0) {
                    data[attrMin  * numAttrs + attrIndex + 4] = value;
                    data[attrMax  * numAttrs + attrIndex + 4] = value;
                    data[attrMean * numAttrs + attrIndex + 4] = value;
                } else {
                    data[attrMin  * numAttrs + attrIndex + 4]  = glm::min(value, data[attrMin  * numAttrs + attrIndex + 4]);
                    data[attrMax  * numAttrs + attrIndex + 4]  = glm::max(value, data[attrMax  * numAttrs + attrIndex + 4]);
                    data[attrMean * numAttrs + attrIndex + 4] += value;
                }
            }

            // nodeId
            {
                int index = (nodeIndex + 4) * numAttrs + 5;
                float value = node->id();
                data[index] = value;

                if (nodeIndex == 0) {
                    data[attrMin  * numAttrs + attrIndex + 5] = value;
                    data[attrMax  * numAttrs + attrIndex + 5] = value;
                    data[attrMean * numAttrs + attrIndex + 5] = value;
                } else {
                    data[attrMin  * numAttrs + attrIndex + 5]  = glm::min(value, data[attrMin  * numAttrs + attrIndex + 5]);
                    data[attrMax  * numAttrs + attrIndex + 5]  = glm::max(value, data[attrMax  * numAttrs + attrIndex + 5]);
                    data[attrMean * numAttrs + attrIndex + 5] += value;
                }
            }

            // lineId
            // is added later
        }

        attrIndex += numStaticAttrs;
    }

    // Add node attributes
    const std::vector<std::string> & nodeAttrs = dataset.nodeAttributes();
    int numNodeAttrs = nodeAttrs.size();

    for (int i=0; i<numNodeAttrs; i++) {
        std::string name = "Node - " + nodeAttrs[i];
        m_attrs.push_back(name);
    }

    // Fill in node attribute values
    const std::vector<Node*> & nodes = dataset.nodes();
    bool first = true;
    for (Node * node : nodes)
    {
        int nodeIndex = node->id();

        for (int i=0; i<numNodeAttrs; i++)
        {
            int index = (nodeIndex + 4) * numAttrs + attrIndex + i;
            float value = node->attribute(i);
            data[index] = value;

            if (first) {
                data[attrMin  * numAttrs + attrIndex + i] = value;
                data[attrMax  * numAttrs + attrIndex + i] = value;
                data[attrMean * numAttrs + attrIndex + i] = value;
            } else {
                if (value < data[attrMin * numAttrs + attrIndex + i]) {
                    data[attrMin * numAttrs + attrIndex + i] = value;
                }
                if (value > data[attrMax * numAttrs + attrIndex + i]) {
                    data[attrMax * numAttrs + attrIndex + i] = value;
                }

                data[attrMean * numAttrs + attrIndex + i] += value;
            }
        }

        first = false;
    }

    attrIndex += numNodeAttrs;

    // Save number of static and node attributes
    m_numNodeAttributes = numStaticAttrs + numNodeAttrs;



    // Add line attributes
    const std::vector<std::string> & lineAttrs = dataset.lineAttributes();
    int numLineAttrs = lineAttrs.size();

    for (int i=0; i<numLineAttrs; i++) {
        std::string name = "Line - " + lineAttrs[i];
        m_attrs.push_back(name);
    }

    // Fill in line attributes
    const std::vector<Line*> & lines = dataset.lines();
    first = true;
    for (Line * line : lines)
    {
        int lineIndex = line->id();

        // Add line ID for each of its nodes
        for (Node * node : line->nodes()) {
            int index = (node->id() + 4) * numAttrs + 6;
            float value = lineIndex;

            data[index] = value;

            if (data[attrMin  * numAttrs + 6] == -1.0) {
                data[attrMin  * numAttrs + 6] = value;
                data[attrMax  * numAttrs + 6] = value;
                data[attrMean * numAttrs + 6] = value;
            } else {
                if (value < data[attrMin * numAttrs + 6]) {
                    data[attrMin * numAttrs + 6] = value;
                }
                if (value > data[attrMax * numAttrs + 6]) {
                    data[attrMax * numAttrs + 6] = value;
                }

                data[attrMean * numAttrs + 6] += value;
            }
        }

        // Line attributes
        for (int i=0; i<numLineAttrs; i++)
        {
            int index = (lineIndex + 4) * numAttrs + attrIndex + i;
            float value = line->attribute(i);
            data[index] = value;

            if (first) {
                data[attrMin  * numAttrs + attrIndex + i] = value;
                data[attrMax  * numAttrs + attrIndex + i] = value;
                data[attrMean * numAttrs + attrIndex + i] = value;
            } else {
                if (value < data[attrMin * numAttrs + attrIndex + i]) {
                    data[attrMin * numAttrs + attrIndex + i] = value;
                }
                if (value > data[attrMax * numAttrs + attrIndex + i]) {
                    data[attrMax * numAttrs + attrIndex + i] = value;
                }

                data[attrMean * numAttrs + attrIndex + i] += value;
            }
        }

        first = false;
    }

    attrIndex += numLineAttrs;


// [TODO] Hier weiter


    // Depending on the index of the attribute, get the associated number of elements with it
    // (e.g., if this is a node attribute, return the number of nodes, if it is an edge attribute, return the number of edges)
    int numNodes = dataset.nodes().size();
    int numLines = dataset.lines().size();
    auto getCount = [&] (int i) -> int {
        int num = 1;
        if (i < numStaticAttrs + numNodeAttrs) {
            num = numNodes;
        } else if (i < numStaticAttrs + numNodeAttrs + numLineAttrs) {
            num = numLines;
        }
        return num;
    };

    auto getAttrType = [&] (int i) -> int
    {
        if (i < numStaticAttrs + numNodeAttrs) {
            return 1; // Nodes
        } else if (i < numStaticAttrs + numNodeAttrs + numLineAttrs) {
            return 2; // Lines
        }
        return 0;
    };

    auto getItemType = [&] (int i) -> int
    {
        if (i < numNodes) {
            return 1; // Nodes
        } else if (i < numNodes + numLines) {
            return 2; // Lines
        }
        return 0;
    };

    // Calculate mean value for each attribute
    for (int i=0; i<numAttrs; i++) {
        data[attrMean * numAttrs + i] /= getCount(i);
    }

    // Calculate standard deviation for each attribute
    for (int i=0; i<numAttrs; i++) {
        float stdDeviation = 0.0f;
        for (int itemIndex=0; itemIndex<numItems; itemIndex++) {
            if (getAttrType(i) == getItemType(itemIndex)) {
                int index = (itemIndex + 4) * numAttrs + i;
                float value = data[index];
                float difference = value - data[attrMean * numAttrs + i];
                stdDeviation += difference * difference;
            }
        }

        stdDeviation /= getCount(i);
        data[attrDev * numAttrs + i] = sqrt(stdDeviation);
    }

    // [DEBUG]
    /*
    for (int i=0; i<m_attrs.size(); i++) {
        float min  = data[attrMin  * numAttrs + i];
        float max  = data[attrMax  * numAttrs + i];
        float mean = data[attrMean * numAttrs + i];
        float dev  = data[attrDev  * numAttrs + i];
        std::cout << std::fixed << "[" << m_attrs[i].toStdString() << "] " << "min: " << min << ", max: " << max << ", mean: " << mean << ", dev: " << dev << "\n";
    }
    */

    // Create texture
    m_texture = new globjects::Texture(gl::GL_TEXTURE_BUFFER);
    globjects::Buffer * buffer = new globjects::Buffer();
    buffer->setData(numFloats * sizeof(float), data, gl::GL_STATIC_DRAW);
    m_texture->texBuffer(gl::GL_R32F, buffer);

    // Release texture data
    free(data);
}

unsigned int AttributeStorage::size() const
{
    return m_size;
}

globjects::Texture * AttributeStorage::texture()
{
    return m_texture;
}

const globjects::Texture * AttributeStorage::texture() const
{
    return m_texture;
}

const std::vector<std::string> & AttributeStorage::attributes() const
{
    return m_attrs;
}

int AttributeStorage::numNodeAttributes() const
{
    return m_numNodeAttributes;
}
