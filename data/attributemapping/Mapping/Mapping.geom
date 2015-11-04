
#version 330
#extension GL_ARB_uniform_buffer_object : enable

layout (points) in;
layout (triangle_strip, max_vertices = 40) out;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Style Transfer Configuration
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_CONFIGURATIONS  16
#define NUM_LOD              4

struct Configuration {
    float radius;               // Index of attribute mapped to tube radius
    float color;                // Index of attribute mapped to color
    float textureX;             // Index of attribute mapped to texture coordinate X
    float textureY;             // Index of attribute mapped to texture coordinate Y
    float textureID;            // ID of used texture
    float colorMapID;           // ID of used color map
    float minRadius;            // Minimal radius
    float maxRadius;            // Maximal radius
    float sizeOfSingleStretch;  // Length of a texture repetition with single stretching
    float sizeOfSingleTorsion;  // Value change leading to a complete 360° texture distortion
    float animationSpeed;       // Animation speed of the texture (texture repetitions/second)
    float tesselation;          // Number of corners of each polygon
    vec3  upVector;             // Vector used to align the polygons
    float geometryType;         // Geometry type: 0 = discard, 1 = tubes, 2 = sphere
    float alpha;                // Alpha value (transparency)
    float positionX;            // Index of attribute mapped to position coordinate X
    float positionY;            // Index of attribute mapped to position coordinate Y
    float positionZ;            // Index of attribute mapped to position coordinate Z
};

layout(std140) uniform CONFIG
{
    Configuration config[MAX_CONFIGURATIONS];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////////////////////////////////////////////////////

// Model view and projection matrix by OSG
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;

// Options
uniform bool  useViewFrustumCulling = true;
uniform int   classIndex = 0;
uniform float STCFactor  = 0.0;

// Current time stamp for animations [seconds]
uniform float time = 0.0;

// Time filter
uniform vec2 timeSpan1;

// ID filter
uniform float filterMin = -1.0;
uniform float filterMax = -1.0;

// Selection
uniform float selectedId;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Input from vertex shader
////////////////////////////////////////////////////////////////////////////////////////////////////

// Node-ID / Line-ID / Time / -
in vec4 v_attrs[1];


////////////////////////////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
////////////////////////////////////////////////////////////////////////////////////////////////////

// Global
flat out float g_textureID;
flat out float g_colorMapID;
flat out float g_geometryType;
flat out float g_alpha;
flat out float g_lineId;

// For tubes
out float g_color;
out vec2  g_texcoord;

// For spheres
out vec3  g_pos;
out vec3  g_center;
out vec3  g_up;
out float g_radius;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////////////////

// Description of a single vertex
struct Vertex {
    vec4  position;
    int   lineId;
    float radius;
    float color;
    float textureX;
    float textureY;
    float textureID;
    float colorMapID;
    float minRadius;
    float maxRadius;
    float sizeOfSingleStretch;
    float sizeOfSingleTorsion;
    float animationSpeed;
    float tesselation;
    float geometryType;
    float alpha;
    vec3  upVector;
};

// Vertices
Vertex vertex[3];


////////////////////////////////////////////////////////////////////////////////////////////////////
// Imported functions
////////////////////////////////////////////////////////////////////////////////////////////////////

vec4  calculatePosition(vec3 inVector);
float minAttributeValue(int attr);
float attributeValue(int nodeIndex, int edgeIndex, int attr);
float normalizedAttribute(int nodeIndex, int edgeIndex, int attr);
float getLinearDepth(vec4 pos);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Get config value for vertex <index> (either direct value or mapped to attribute)
//   if input >= 0, the value itself is the config value
//   if input <  0, the value of attribute[-input] is used
float configValue(int nodeIndex, int lineIndex, float input)
{
    if (input >= 0.0) {
        return input;
    } else {
        float value = attributeValue(nodeIndex, lineIndex, int(-input) - 1);

        // Space-Time Cube
        if (input == -8.0) {
            value -= minAttributeValue(int(-input) - 1) + timeSpan1.x * 1.0;
        }

        return value;
    }
}

float normalizedConfigValue(int nodeIndex, int lineIndex, float input)
{
    if (input >= 0.0) {
        return input;
    } else {
        float value = normalizedAttribute(nodeIndex, lineIndex, int(-input) - 1);
        return value;
    }
}

// Get configuration ID for given class ID and LOD level
int getConfigID(int classID, int lod)
{
    return classID * NUM_LOD + lod;
}

// Get LOD for given depth value
int getLOD(float depth)
{
    return depth < 0.05 ? 0 : (
           depth < 0.08 ? 1 : 2);
}

// Check if a vertex (object space) is inside the view frustum
bool viewFrustumCulling(const in vec4 vertex)
{
    // Return 'true' if the vertex is culled (not visible), else 'false'
    vec4 V = modelViewProjectionMatrix * vertex;
    return !( ((-V.w < V.x) && (V.x < V.w)) && 
              ((-V.w < V.y) && (V.y < V.w)) && 
              ((-V.w < V.z) && (V.z < V.w)) );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry #1: Tubes
////////////////////////////////////////////////////////////////////////////////////////////////////

void transformTube()
{
    // We're rendering tubes
    g_geometryType = 1.0;

    // Ignore segment if this is the last point on a line
    if (vertex[0].lineId == vertex[1].lineId) {
        float pi = 2.0 * asin(1.0);
        g_alpha  = vertex[0].alpha;
        g_lineId = vertex[0].lineId;

        vec3 direction[2];
        vec4 polygonH[2];
        vec4 polygonW[2];

vec3 upVector = normalize(vertex[0].position.xyz);

        // Compute two vectors in the planes, one up and one perpendicular
        for (int i = 0; i < 2; i++) {
            direction[i] = (vertex[i+1].position - vertex[i].position).xyz;
            polygonW[i]  = vec4(normalize(cross(direction[i], upVector)), 0.0);
            polygonH[i]  = vec4(normalize(cross(direction[i], polygonW[i].xyz)), 0.0);
        }

        // Compute the radius based on the user-defined input attribute
        float radius[2];
        for (int j = 0; j < 2; j++)
            radius[j] = mix(vertex[j].minRadius, vertex[j].maxRadius, vertex[j].radius);

        // Get polygon size
        int tesselation = int(vertex[0].tesselation);
        int numberOfVertices = (tesselation == 2) ? 2 : tesselation + 1;

        // Trace a circle and create the polygons
        for (int i = 0; i < numberOfVertices; i++) {
            for (int j = 0; j < 2; j++) {
                float angle = (float(i) / tesselation) * 2 * pi;

                // Compute the position of the polygon corner
                gl_Position = modelViewProjectionMatrix *
                                (vertex[j].position
                                + polygonW[j] * cos(angle) * radius[j]
                                + polygonH[j] * sin(angle) * radius[j]);

                g_color = vertex[j].color;

                float torsionFactor =
                    (vertex[j].sizeOfSingleTorsion > 0) ? 1.0 / vertex[j].sizeOfSingleTorsion : 0.0;

                // Apply the texture respecting stretch and torsion
                g_texcoord = vec2(
                    vertex[j].textureX / vertex[0].sizeOfSingleStretch - time * vertex[0].animationSpeed,
                    float(i) / tesselation + vertex[j].textureY * torsionFactor
                );

                // Pass options on to fragment shader
                g_textureID  = vertex[0].textureID;
                g_colorMapID = vertex[0].colorMapID;
                g_alpha      = vertex[0].alpha;
                g_lineId     = vertex[0].lineId;

                EmitVertex();
            }
        }

        EndPrimitive();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry #2: Spheres
////////////////////////////////////////////////////////////////////////////////////////////////////

void transformSphere()
{
    // We're rendering spheres
    g_geometryType = 2.0;

    // Pass options on to fragment shader
    g_textureID  = vertex[0].textureID;
    g_colorMapID = vertex[0].colorMapID;
    g_color      = vertex[0].color;
    g_alpha      = vertex[0].alpha;
    g_lineId     = vertex[0].lineId;

    // Define sphere: center and radius
    vec3  M = (viewMatrix * vertex[0].position).xyz;
    float r = mix(vertex[0].minRadius, vertex[0].maxRadius, vertex[0].radius);
    vec3  u = -M;

    // Calculate billboard
    vec3 dir = normalize(M);
    vec3 B   = M - dir*r;
    vec3 dx  = cross(vec3(0.0, 1.0, 0.0), dir) * r;
    vec3 dy  = cross(dx, dir) * r;

    g_pos       = B-dx+dy;
    g_center    = M;
    g_up        = u;
    g_radius    = r;
    gl_Position = projectionMatrix * vec4(g_pos, 1.0);
    EmitVertex();

    g_pos       = B+dx+dy;
    g_center    = M;
    g_up        = u;
    g_radius    = r;
    gl_Position = projectionMatrix * vec4(g_pos, 1.0);
    EmitVertex();

    g_pos       = B-dx-dy;
    g_center    = M;
    g_up        = u;
    g_radius    = r;
    gl_Position = projectionMatrix * vec4(g_pos, 1.0);
    EmitVertex();

    g_pos       = B+dx-dy;
    g_center    = M;
    g_up        = u;
    g_radius    = r;
    gl_Position = projectionMatrix * vec4(g_pos, 1.0);
    EmitVertex();

    EndPrimitive();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////

// Calculate vertex position
void calculateVertexPosition(int nodeIndex, int i, int lineIndex, int configID)
{
    // Get index into attribute array
    int ind = nodeIndex + i;

    // Get position of vertex according to configuration
    float x = configValue(ind, lineIndex, config[configID].positionX);
    float y = configValue(ind, lineIndex, config[configID].positionY);
    float z = configValue(ind, lineIndex, config[configID].positionZ);
    vec4 pos1 = calculatePosition( vec3(x, y, z) );

    // Calculate position WITHOUT space-time-cube
    float posX = configValue(ind, lineIndex, -3);
    float posY = configValue(ind, lineIndex, -4);
    float posZ = configValue(ind, lineIndex, -5);
    vec4  pos2 = calculatePosition( vec3(posX, posY, posZ) );

    // Interpolate between non-STC and STC positions
    vertex[i].position = mix(pos2, pos1, STCFactor);
}

// Calculate vertex attributes
void calculateVertexAttributes(int nodeIndex, int i, int lineIndex, int configID)
{
    // Get index into attribute array
    int ind = nodeIndex + i;
    int lin = int(attributeValue(ind, lineIndex, 6));

    // Get vertex attributes according to configuration
    vertex[i].lineId              = lin;
    vertex[i].radius              = normalizedConfigValue(ind, lin, config[configID].radius);
    vertex[i].color               = normalizedConfigValue(ind, lin, config[configID].color);
    vertex[i].textureX            = configValue(ind, lin, config[configID].textureX);
    vertex[i].textureY            = configValue(ind, lin, config[configID].textureY);
    vertex[i].textureID           = configValue(ind, lin, config[configID].textureID);
    vertex[i].colorMapID          = configValue(ind, lin, config[configID].colorMapID);
    vertex[i].minRadius           = configValue(ind, lin, config[configID].minRadius);
    vertex[i].maxRadius           = configValue(ind, lin, config[configID].maxRadius);
    vertex[i].sizeOfSingleStretch = configValue(ind, lin, config[configID].sizeOfSingleStretch);
    vertex[i].sizeOfSingleTorsion = configValue(ind, lin, config[configID].sizeOfSingleTorsion);
    vertex[i].animationSpeed      = configValue(ind, lin, config[configID].animationSpeed);
    vertex[i].tesselation         = configValue(ind, lin, config[configID].tesselation);
    vertex[i].geometryType        = configValue(ind, lin, config[configID].geometryType);
    vertex[i].alpha               = normalizedConfigValue(ind, lin, config[configID].alpha);
    vertex[i].upVector            = config[configID].upVector;
}

void main()
{
    // Fetch vertex data
    float nodeId  = v_attrs[0].x;
    float lineId  = v_attrs[0].y;
    float time    = v_attrs[0].z;
    bool  visible = true;

    // Attribute filtering
    visible = true;

    // Temporal filtering
    float visibleTime1   = true;
    float visibleTime2   = true;
    bool  contextVisible = false;

    // ID filtering
    if (visible && filterMin > -1 && filterMax > -1 && (lineId < filterMin || lineId > filterMax)) {
        visible = false;
    }

    // Get class ID
    int classID = 0;
    if (classIndex == -1) {
        // Temporal focus+context

        // Choose class by temporal focus+context
             if (visibleTime1 > 0.0 && visibleTime2 > 0.0) classID = 3;
        else if (visibleTime2 > 0.0)                       classID = 2;
        else if (visibleTime1 > 0.0)                       classID = 1;
        else                                               classID = 0;

        // To also visualize the context, everything has to be visible
        contextVisible = true;
    } else {
        // Choose class ID by looking at the specified attribute
        classID = int(attributeValue(int(nodeId), int(lineId), classIndex));
    }

    // Check if element is selected
    bool isSelected = (lineId > 0 && lineId == selectedId);

    // Get configuration from class ID, ignore LOD because we don't know the position, yet
    int configID = 0;
    if (isSelected) {
        configID = getConfigID(classID, 3);
    } else {
        configID = getConfigID(classID, 0);
    }

    // Calculate position of the vertex (ignoring LOD)
    calculateVertexPosition(int(nodeId), 0, int(lineId), int(configID));

    // Get configuration from class ID and LOD
    if (isSelected) {
        configID = getConfigID(classID, 3);
    } else {
        configID = getConfigID(classID, getLOD(getLinearDepth(viewMatrix * vertex[0].position)));
    }

    // Check visibility
    visible = visible &&
              (visibleTime1 > 0.0 || visibleTime2 > 0.0 || contextVisible) &&
              (!useViewFrustumCulling || !viewFrustumCulling(vertex[0].position));
    if (visible) {
        // Process 3 vertices: the current and the next two
        for (int i=0; i<3; i++) {
            calculateVertexPosition  (int(nodeId), i, int(lineId), int(configID));
            calculateVertexAttributes(int(nodeId), i, int(lineId), int(configID));
        }

        // Create geometry
        if (vertex[1].geometryType >= 2.0) {
            transformSphere();
        } else if (vertex[1].geometryType <= 1.0) {
            transformTube();
        }
    }
}
