
#include "AttributeMappingPainter.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>

#include <globjects/globjects.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/Buffer.h>
#include <globjects/UniformBlock.h>

#include <gloperate/painter/TargetFramebufferCapability.h>
#include <gloperate/painter/ViewportCapability.h>
#include <gloperate/painter/PerspectiveProjectionCapability.h>
#include <gloperate/painter/CameraCapability.h>
#include <gloperate/painter/VirtualTimeCapability.h>
#include <gloperate/resources/ResourceManager.h>
#include <gloperate/primitives/AdaptiveGrid.h>

#include "MappingConfigList.h"
#include "MappingConfig.h"
#include "DataSet.h"
#include "Line.h"
#include "Node.h"
#include "Tools.h"


using namespace gl;
using namespace glm;
using namespace globjects;


AttributeMappingPainter::AttributeMappingPainter(gloperate::ResourceManager & resourceManager, const reflectionzeug::Variant & pluginInfo)
: Painter("AttributeMapping", resourceManager, pluginInfo)
, m_targetFramebufferCapability(addCapability(new gloperate::TargetFramebufferCapability()))
, m_viewportCapability(addCapability(new gloperate::ViewportCapability()))
, m_projectionCapability(addCapability(new gloperate::PerspectiveProjectionCapability(m_viewportCapability)))
, m_cameraCapability(addCapability(new gloperate::CameraCapability()))
, m_virtualTimeCapability(addCapability(new gloperate::VirtualTimeCapability()))
, m_propLines(nullptr)
, m_propMapping(nullptr)
, m_linesVisible(false)
, m_colorMap("color_gradient.png")
, m_lineColor("Zero")
, m_lineWidth("Zero")
, m_nodeHeight("PosY")
, m_mappingVisible(true)
, m_dataSet(nullptr)
, m_configs(nullptr)
{
    // List available color maps
    m_colorMaps.push_back("color_gradient.png");
    m_colorMaps.push_back("0000_diverging_red_green.png");
    m_colorMaps.push_back("0001_diverging_blue_yellow.png");
    m_colorMaps.push_back("0002_diverging_green_violet.png");
    m_colorMaps.push_back("0003_diverging_violet_orange.png");
    m_colorMaps.push_back("0004_diverging_cyan_mauve.png");
    m_colorMaps.push_back("0005_hsv_saturation_orange.png");
    m_colorMaps.push_back("0006_bbr_cool.png");

    // List available texture maps
    m_textureMaps.push_back("tube_texture_none.png");
    m_textureMaps.push_back("tube_texture_arrow.png");
    m_textureMaps.push_back("tube_texture_grid.png");
    m_textureMaps.push_back("tube_texture_daytime.png");

    // Register properties
    m_propLines = new PropertyGroup("Lines");
    addProperty(m_propLines);

    m_propLines->addProperty<bool>("Visible", this, &AttributeMappingPainter::linesVisible, &AttributeMappingPainter::setLinesVisible);

    m_propLines->addProperty<std::string>("ColorMap", this, &AttributeMappingPainter::getColorMap, &AttributeMappingPainter::setColorMap);
    m_propLines->property("ColorMap")->setOption("choices", m_colorMaps);

    m_propLines->addProperty<std::string>("LineColor", this, &AttributeMappingPainter::getLineColor, &AttributeMappingPainter::setLineColor);
    m_propLines->property("LineColor")->setOption("choices", std::vector<std::string>());

    m_propLines->addProperty<std::string>("LineWidth",  this, &AttributeMappingPainter::getLineWidth,  &AttributeMappingPainter::setLineWidth);
    m_propLines->property("LineWidth")->setOption("choices", std::vector<std::string>());

    m_propLines->addProperty<std::string>("NodeHeight", this, &AttributeMappingPainter::getNodeHeight,  &AttributeMappingPainter::setNodeHeight);
    m_propLines->property("NodeHeight")->setOption("choices", std::vector<std::string>());

    m_propMapping = new PropertyGroup("AttributeMapping");
    addProperty(m_propMapping);

    m_propMapping->addProperty<bool>("Visible", this, &AttributeMappingPainter::mappingVisible, &AttributeMappingPainter::setMappingVisible);

    // Create mapping configuration and expose it as a property tree
    m_configs = new MappingConfigList();
    m_configs->setColorMaps(m_colorMaps);
    m_configs->setTextureMaps(m_textureMaps);
    m_configs->setNumConfigs(8);
    for (int i=0; i<8; i++)
    {
        // Give each configuration a different color map
        m_configs->getConfig(i)->setColorMapID(i);

        // Every second configuration renders spheres instead of tubes
        m_configs->getConfig(i)->setGeometryType((i % 2) + 1);
    }
    addProperty(m_configs);
}

AttributeMappingPainter::~AttributeMappingPainter()
{
}

bool AttributeMappingPainter::linesVisible() const
{
    return m_linesVisible;
}

void AttributeMappingPainter::setLinesVisible(bool visible)
{
    m_linesVisible = visible;
}

std::string AttributeMappingPainter::getColorMap() const
{
    return m_colorMap;
}

void AttributeMappingPainter::setColorMap(const std::string & colorMap)
{
    m_colorMap = colorMap;
    m_colorMapTexture = nullptr;
}

std::string AttributeMappingPainter::getLineColor() const
{
    return m_lineColor;
}

void AttributeMappingPainter::setLineColor(const std::string & attr)
{
    m_lineColor = attr;
}

std::string AttributeMappingPainter::getLineWidth() const
{
    return m_lineWidth;
}

void AttributeMappingPainter::setLineWidth(const std::string & attr)
{
    m_lineWidth = attr;
}

std::string AttributeMappingPainter::getNodeHeight() const
{
    return m_nodeHeight;
}

void AttributeMappingPainter::setNodeHeight(const std::string & attr)
{
    m_nodeHeight = attr;
}

bool AttributeMappingPainter::mappingVisible() const
{
    return m_mappingVisible;
}

void AttributeMappingPainter::setMappingVisible(bool visible)
{
    m_mappingVisible = visible;
}

void AttributeMappingPainter::onInitialize()
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

    // Create texture array for color maps
    createColorMaps();

    // Create texture array for textures
    createTextureMaps();

    // Create program for line rendering
    m_programLines = new Program();
    m_programLines->attach(
        Shader::fromFile(GL_VERTEX_SHADER,   "data/attributemapping/shaders/lines/Lines.vert"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/lines/Lines.geom"),
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/attributemapping/shaders/lines/Lines.frag"),
        Shader::fromFile(GL_VERTEX_SHADER,   "data/attributemapping/shaders/Position.glsl"),
        Shader::fromFile(GL_VERTEX_SHADER,   "data/attributemapping/shaders/Attributes.glsl"),
        Shader::fromFile(GL_VERTEX_SHADER,   "data/attributemapping/shaders/Filtering.glsl"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/ScreenSize.glsl"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/ColorMap.glsl")
    );

    // Create program for real-time attribute mapping
    m_programMapping = new Program();
    m_programMapping->attach(
        Shader::fromFile(GL_VERTEX_SHADER,   "data/attributemapping/shaders/mapping/Mapping.vert"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/mapping/Mapping.geom"),
        Shader::fromFile(GL_FRAGMENT_SHADER, "data/attributemapping/shaders/mapping/Mapping.frag"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/Position.glsl"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/Depth.glsl"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/Attributes.glsl"),
        Shader::fromFile(GL_GEOMETRY_SHADER, "data/attributemapping/shaders/Filtering.glsl")
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

void AttributeMappingPainter::onPaint()
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

    // Get camera status
    const auto viewMatrix                = m_cameraCapability->view();
    const auto projectionMatrix          = m_projectionCapability->projection();
    const auto modelViewProjectionMatrix = projectionMatrix * viewMatrix;
    const auto eye                       = m_cameraCapability->eye();

    // Clear image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set render states
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // Render grid
    m_grid->update(eye, modelViewProjectionMatrix);
    m_grid->draw();

    // Update mapping configuration
    if (m_configs->checkUpdated() || !m_configDataTex.get()) {
        uploadConfig();
    }

    // Update color map texture
    if (!m_colorMapTexture.get())
    {
        m_colorMapTexture = m_resourceManager.load<globjects::Texture>("data/attributemapping/gradients/" + m_colorMap);
    }

    // Bind attribute texture
    m_attrStorage->texture()->bindActive(gl::GL_TEXTURE0);
    gl::glActiveTexture(gl::GL_TEXTURE0);
    m_attrStorage->texture()->bind();
    m_programLines  ->setUniform("attributes", 0);
    m_programMapping->setUniform("attributes", 0);

    // Render lines
    if (m_linesVisible)
    {
        // Bind program
        m_programLines->use();

        // Bind color map texture
        m_attrStorage->texture()->bindActive(gl::GL_TEXTURE0 + 1);
        gl::glActiveTexture(gl::GL_TEXTURE0 + 1);
        m_colorMapTexture->bind();
        m_programLines->setUniform("colorMap", 1);

        // Update shader uniforms
        m_programLines->setUniform("screenSize",                glm::vec2(m_viewportCapability->width(), m_viewportCapability->height()));
        m_programLines->setUniform("modelViewProjectionMatrix", modelViewProjectionMatrix);
        m_programLines->setUniform("lineColor",                 Tools::indexOf(m_attributes, m_lineColor)  );
        m_programLines->setUniform("lineWidth",                 Tools::indexOf(m_attributes, m_lineWidth)  );
        m_programLines->setUniform("nodeHeight",                Tools::indexOf(m_attributes, m_nodeHeight) );
        m_programLines->setUniform("numNodeAttributes",         m_attrStorage->numNodeAttributes());

        // Draw geometry
        glLineWidth(10.0f);
        m_lineGeometry->draw();
        glLineWidth(1.0f);

        // Release program
        m_programLines->release();
    }

    // Render real-time attribute mapping
    if (m_mappingVisible)
    {
        // Bind program
        m_programMapping->use();

        // Bind color maps texture
        m_colorMapsTex->bindActive(gl::GL_TEXTURE0 + 1);
        gl::glActiveTexture(gl::GL_TEXTURE0 + 1);
        m_colorMapsTex->bind();
        m_programMapping->setUniform("colorMaps", 1);

        // Bind texture maps texture
        m_textureMapsTex->bindActive(gl::GL_TEXTURE0 + 2);
        gl::glActiveTexture(gl::GL_TEXTURE0 + 2);
        m_textureMapsTex->bind();
        m_programMapping->setUniform("textures", 2);

        // Bind configuration texture
        m_configDataTex->bindActive(gl::GL_TEXTURE0 + 3);
        gl::glActiveTexture(gl::GL_TEXTURE0 + 3);
        m_configDataTex->bind();
        m_programMapping->setUniform("config", 3);

        // Update shader uniforms
        m_programMapping->setUniform("viewMatrix",                viewMatrix);
        m_programMapping->setUniform("projectionMatrix",          projectionMatrix);
        m_programMapping->setUniform("modelViewProjectionMatrix", modelViewProjectionMatrix);
        m_programMapping->setUniform("numNodeAttributes",         m_attrStorage->numNodeAttributes());
        m_programMapping->setUniform("classIndex",                m_configs->classificationIndex());
        m_programMapping->setUniform("time",                      m_virtualTimeCapability->time());

        // Draw geometry
        m_nodeGeometry->draw();

        // Release program
        m_programMapping->release();
    }

    // Release framebuffer
    Framebuffer::unbind(GL_FRAMEBUFFER);
}

void AttributeMappingPainter::generateTestData()
{
    // Destroy old data
    if (m_dataSet) {
        delete m_dataSet;
    }

    // Create new data set
    m_dataSet = new DataSet;

    std::vector<std::string> lineAttributes = { "LineClass" };
    m_dataSet->setLineAttributes(lineAttributes);

    std::vector<std::string> nodeAttributes = { "Linear", "Sinus", "HeightClass" };
    m_dataSet->setNodeAttributes(nodeAttributes);

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
        line->setAttribute(0, i%4);
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
            node->setAttribute(0, (float)j / (float)(numNodes-1));
            node->setAttribute(1, pos.y);
            node->setAttribute(2, pos.y > 0.12f ? 1.0 : 0.0);
        }
    }

    // Create geometry
    m_nodeGeometry = new NodeGeometry();
    m_nodeGeometry->setData(*m_dataSet);
    m_lineGeometry = new LineGeometry();
    m_lineGeometry->setData(*m_dataSet);

    // Create attribute storage
    m_attrStorage = new AttributeStorage();
    m_attrStorage->setData(*m_dataSet);

    // Update attribute choices
    m_attributes = m_attrStorage->attributes();
//    m_attributes.insert(m_attributes.begin(), "None");
    m_propLines->property("LineColor") ->setOption("choices", m_attributes);
    m_propLines->property("LineWidth") ->setOption("choices", m_attributes);
    m_propLines->property("NodeHeight")->setOption("choices", m_attributes);
    m_configs->setAttributes(m_attributes);
}

void AttributeMappingPainter::createTextureMaps()
{
    // Merge all textures into a single 2D texture and upload it as a 2D texture array

    int width  = 256;
    int height = 256;

    size_t numImages = m_textureMaps.size();

    unsigned char * buffer = new unsigned char[numImages * width * height * 4];

    for (size_t i=0; i<numImages; i++)
    {
        std::string filename = "data/attributemapping/textures/" + m_textureMaps[numImages - 1 - i];

        globjects::Texture * texture = m_resourceManager.load<globjects::Texture>(filename);
        if (texture) {
            std::vector<unsigned char> image = texture->getImage(0, GL_RGBA, GL_UNSIGNED_BYTE);

            for (int y=0; y<height; y++)
            {
                for (int x=0; x<width; x++)
                {
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 0] = image[((height - 1 - y) * width + x) * 4 + 0];
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 1] = image[((height - 1 - y) * width + x) * 4 + 1];
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 2] = image[((height - 1 - y) * width + x) * 4 + 2];
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 3] = image[((height - 1 - y) * width + x) * 4 + 3];
                }
            }
        }
    }

    // Create texture
    globjects::Texture * texture = new globjects::Texture(gl::GL_TEXTURE_2D_ARRAY);
    texture->storage3D(1, gl::GL_RGBA8, width, height, numImages);
    texture->subImage3D(0, 0, 0, 0, width, height, numImages, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, buffer);
    texture->setParameter(gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
    texture->setParameter(gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
    texture->setParameter(gl::GL_TEXTURE_WRAP_S,     gl::GL_REPEAT);
    texture->setParameter(gl::GL_TEXTURE_WRAP_T,     gl::GL_REPEAT);
    m_textureMapsTex = texture;

    delete [] buffer;
}

void AttributeMappingPainter::createColorMaps()
{
    // Merge all textures into a single 2D texture and upload it as a 2D texture array

    int width  = 128;
    int height = 1;

    size_t numImages = m_colorMaps.size();

    unsigned char * buffer = new unsigned char[numImages * width * height * 4];

    for (size_t i=0; i<numImages; i++)
    {
        std::string filename = "data/attributemapping/gradients/" + m_colorMaps[numImages - 1 - i];

        globjects::Texture * texture = m_resourceManager.load<globjects::Texture>(filename);
        if (texture) {
            std::vector<unsigned char> image = texture->getImage(0, GL_RGBA, GL_UNSIGNED_BYTE);

            for (int y=0; y<height; y++)
            {
                for (int x=0; x<width; x++)
                {
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 0] = image[((height - 1 - y) * width + x) * 4 + 0];
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 1] = image[((height - 1 - y) * width + x) * 4 + 1];
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 2] = image[((height - 1 - y) * width + x) * 4 + 2];
                    buffer[((numImages - 1 - i) * width * height + y * width + x) * 4 + 3] = image[((height - 1 - y) * width + x) * 4 + 3];
                }
            }
        }
    }

    // Create texture
    globjects::Texture * texture = new globjects::Texture(gl::GL_TEXTURE_2D_ARRAY);
    texture->storage3D(1, gl::GL_RGBA8, width, height, numImages);
    texture->subImage3D(0, 0, 0, 0, width, height, numImages, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, buffer);
    texture->setParameter(gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
    texture->setParameter(gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
    texture->setParameter(gl::GL_TEXTURE_WRAP_S,     gl::GL_CLAMP_TO_EDGE);
    texture->setParameter(gl::GL_TEXTURE_WRAP_T,     gl::GL_CLAMP_TO_EDGE);
    m_colorMapsTex = texture;

    delete [] buffer;
}

void AttributeMappingPainter::uploadConfig()
{
    std::vector<float> configData;

    // Get configurations
    int numConfigs = m_configs->numConfigs();
    for (int i=0; i<numConfigs; i++) {
        MappingConfig * config = m_configs->getConfig(i);
        if (!config) continue;

        // Get LOD configurations
        for (unsigned int j=1; j<=4; j++) {
            MappingConfig * lod = config->getLodConfig(j);
            if (!lod) continue;

            // Copy configuration data into uniform buffer
            configData.push_back(lod->radius());
            configData.push_back(lod->color());
            configData.push_back(lod->textureX());
            configData.push_back(lod->textureY());
            configData.push_back(lod->textureID());
            configData.push_back(lod->colorMapID());
            configData.push_back(lod->minRadius());
            configData.push_back(lod->maxRadius());
            configData.push_back(lod->stretchSize());
            configData.push_back(lod->torsionSize());
            configData.push_back(lod->animationSpeed());
            configData.push_back((float)lod->tesselation());
            configData.push_back(lod->upVector().x);
            configData.push_back(lod->upVector().y);
            configData.push_back(lod->upVector().z);
            configData.push_back((float)lod->geometryType());
            configData.push_back(lod->alpha());
            configData.push_back(lod->positionX());
            configData.push_back(lod->positionY());
            configData.push_back(lod->positionZ());
        }
    }

    // Create texture buffer
    m_configDataTex = new globjects::Texture(gl::GL_TEXTURE_BUFFER);
    globjects::Buffer * buffer = new globjects::Buffer();
    buffer->setData(configData, gl::GL_STATIC_DRAW);
    m_configDataTex->texBuffer(gl::GL_R32F, buffer);
}
