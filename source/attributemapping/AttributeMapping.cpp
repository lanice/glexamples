
#include "AttributeMapping.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>

#include <globjects/globjects.h>
#include <globjects/Program.h>

#include <gloperate/painter/TargetFramebufferCapability.h>
#include <gloperate/painter/ViewportCapability.h>
#include <gloperate/painter/PerspectiveProjectionCapability.h>
#include <gloperate/painter/CameraCapability.h>

#include <gloperate/primitives/AdaptiveGrid.h>

#include "DataSet.h"
#include "Line.h"
#include "Node.h"


using namespace gl;
using namespace glm;
using namespace globjects;


AttributeMapping::AttributeMapping(gloperate::ResourceManager & resourceManager, const reflectionzeug::Variant & pluginInfo)
: Painter("AttributeMapping", resourceManager, pluginInfo)
, m_targetFramebufferCapability(addCapability(new gloperate::TargetFramebufferCapability()))
, m_viewportCapability(addCapability(new gloperate::ViewportCapability()))
, m_projectionCapability(addCapability(new gloperate::PerspectiveProjectionCapability(m_viewportCapability)))
, m_cameraCapability(addCapability(new gloperate::CameraCapability()))
, m_dataSet(nullptr)
{
}

AttributeMapping::~AttributeMapping()
{
}

void AttributeMapping::onInitialize()
{
    // Apply fix for apple
#ifdef __APPLE__
    Shader::clearGlobalReplacements();
    Shader::globalReplace("#version 140", "#version 150");

    debug() << "Using global OS X shader replacement '#version 140' -> '#version 150'" << std::endl;
#endif

    // Create adaptive grid
    m_grid = new gloperate::AdaptiveGrid();
    m_grid->setColor({0.6f, 0.6f, 0.6f});

    // Create program
    m_program = new Program();
    m_program->attach(
        Shader::fromFile(GL_VERTEX_SHADER,   "data/attributemapping/Lines.vert"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/Lines.geom"),
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/attributemapping/Lines.frag"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/ScreenSize.glsl"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/ColorMap.glsl")
    );

    // Initialize camera
    static const auto zNear = 0.3f, zFar = 15.f, fovy = 50.f;
    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));
    m_grid->setNearFar(zNear, zFar);

    // Set clear color
    glClearColor(0.85f, 0.87f, 0.91f, 1.0f);

    // Create data set
    generateTestData();
}

void AttributeMapping::onPaint()
{
    // Update viewport
    if (m_viewportCapability->hasChanged())
    {
        glViewport(
            m_viewportCapability->x(),
            m_viewportCapability->y(),
            m_viewportCapability->width(),
            m_viewportCapability->height()
        );

        m_viewportCapability->setChanged(false);
    }

    // Get target framebuffer
    auto fbo = m_targetFramebufferCapability->framebuffer();
    if (!fbo) {
        fbo = globjects::Framebuffer::defaultFBO();
    }

    // Bind framebuffer
    fbo->bind(GL_FRAMEBUFFER);

    // Clear image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto modelViewProjection = m_projectionCapability->projection() * m_cameraCapability->view();
    const auto eye = m_cameraCapability->eye();

    // Set states
    glEnable(GL_DEPTH_TEST);

    // Render grid
    m_grid->update(eye, modelViewProjection);
    m_grid->draw();

    // Render sphere
    m_program->use();
    m_program->setUniform("screenSize", glm::vec2(m_viewportCapability->width(), m_viewportCapability->height()));
    m_program->setUniform("modelViewProjectionMatrix", modelViewProjection);
    m_lineGeometry->draw();
    m_program->release();

    // Release framebuffer
    Framebuffer::unbind(GL_FRAMEBUFFER);
}

void AttributeMapping::generateTestData()
{
    // Destroy old data
    if (m_dataSet) {
        delete m_dataSet;
    }

    // Create new data set
    m_dataSet = new DataSet;

    // Define center
    glm::vec3 center;

    // Create radial lines around the center
    int numLines = 16;
    for (int i=0; i<numLines; i++)
    {
        // Calculate direction vector (2D)
        float angle = ((float)i) * (2.0f * glm::pi<float>() / (float)numLines);
        glm::vec3 dir(glm::sin(angle), 0.0f, glm::cos(angle));

        // Create line
        Line * line = new Line();
        m_dataSet->add(line);

        // Calculate nodes on that line
        int numNodes = 100;
        for (int j=0; j<numNodes; j++)
        {
            // Create node
            Node * node = new Node();
            m_dataSet->add(node);
            line->add(node);

            // Calculate node position
            glm::vec3 pos = center + (j * 0.005f) * dir;
            pos.y = glm::sin(glm::pi<float>() * (float)j / (float)(numNodes-1) ) * 0.2f;
            node->setPosition(pos);

        }
    }

    // Create geometry
    m_lineGeometry = new LineGeometry();
    m_lineGeometry->setData(*m_dataSet);
}
