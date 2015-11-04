
#version 330

#define MAX_ATTRIBUTES 8


////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////////////////////////////////////////////////////

// Mapping options
uniform int lineColor  = -1;
uniform int lineWidth  = -1;
uniform int nodeHeight = -1;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Input data
////////////////////////////////////////////////////////////////////////////////////////////////////

// Input vector (x/y/z), might be the position or can be transformed to a position
layout (location = 0) in vec4 position;

// Node-ID / Line-ID / Time / -
layout (location = 1) in vec4 attrs;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Output to geometry shader
////////////////////////////////////////////////////////////////////////////////////////////////////

flat out int   v_nodeId;   // Node ID
flat out int   v_lineId;   // Line ID
     out vec3  v_pos;      // Position (world space)
     out vec3  v_inVector; // Input vector (x/y/z)
     out float v_visible;  // Line visiblity
     out float v_color;    // Line color (0..1, mapped on color map)
     out float v_width;    // Line width (0..1)


////////////////////////////////////////////////////////////////////////////////////////////////////
// Imported functions
////////////////////////////////////////////////////////////////////////////////////////////////////

vec4 calculatePosition(vec3 inVector)
{
    return vec4(inVector, 1.0);
}

/*
float filterAttrs(int nodeIndex, int edgeIndex);
float filterTimespan1(float time);
float filterTimespan2(float time);
float filterTimespan3(float time);
*/
float attributeValue(int nodeIndex, int edgeIndex, int attr);
float normalizedAttribute(int nodeIndex, int edgeIndex, int attr);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shader
////////////////////////////////////////////////////////////////////////////////////////////////////

void main(void)
{
    // Get height
    float h = position.y;
    if (nodeHeight > -1) {
        h = attributeValue(int(attrs.x), int(attrs.y), nodeHeight);
    }

    // Transform point
    v_inVector  = vec3(position.x, h, position.z);
    v_pos       = calculatePosition(v_inVector).xyz;

    // Set attributes
    v_nodeId    = int(attrs.x);
    v_lineId    = int(attrs.y);
    v_visible   = 1.0;
    v_color     = normalizedAttribute(int(attrs.x), int(attrs.y), lineColor);
    v_width     = normalizedAttribute(int(attrs.x), int(attrs.y), lineWidth);
}
