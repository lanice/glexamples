
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

// Position (lat / lon / height) in radians
layout (location = 0) in vec4 position;

// Node-ID / Line-ID / Time / -
layout (location = 1) in vec4 attrs;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Output to geometry shader
////////////////////////////////////////////////////////////////////////////////////////////////////

flat out int   v_nodeId;  // Node ID
flat out int   v_lineId;  // Line ID
     out vec3  v_pos;     // Position (world space)
     out vec3  v_latlon;  // Position (lat/lon/height)
     out float v_visible; // Line visiblity
     out float v_color;   // Line color (0..1, mapped on color map)
     out float v_width;   // Line width (0..1)


////////////////////////////////////////////////////////////////////////////////////////////////////
// Imported functions
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
vec4 fromLatLon(vec3 latLonHeight);
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
    // [TODO]
    /*
    // Get height
    float h = position.z;
    if (nodeHeight > -1) {
        h = attributeValue(int(attrs.x), int(attrs.y), nodeHeight);
    }

    // Transform point
    v_latlon    = vec3(position.x, position.y, h);
    v_pos       = fromLatLon(v_latlon).xyz;

    // Set attributes
    v_nodeId    = int(attrs.x);
    v_lineId    = int(attrs.y);
    v_visible   = filterTimespan1(attrs.z) * filterAttrs(int(attrs.x), int(attrs.y));
    v_color     = normalizedAttribute(int(attrs.x), int(attrs.y), lineColor);
    v_width     = normalizedAttribute(int(attrs.x), int(attrs.y), lineWidth);
    */

    v_latlon    = position.xyz;
    v_pos       = v_latlon.xyz;
    v_nodeId    = int(attrs.x);
    v_lineId    = int(attrs.y);
    v_visible   = 1.0;
    v_color     = normalizedAttribute(int(attrs.x), int(attrs.y), lineColor);
    v_width     = normalizedAttribute(int(attrs.x), int(attrs.y), lineWidth);
}
