
#version 330

#define MAX_VERTICES 16
layout(lines_adjacency) in;
layout(line_strip, max_vertices = MAX_VERTICES) out;

#define M_PI 3.1415926535897932384626433832795


////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////////////////////////////////////////////////////

uniform mat4 modelViewProjectionMatrix;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Input from vertex shader
////////////////////////////////////////////////////////////////////////////////////////////////////

flat in int   v_nodeId[4];  // Node ID
flat in int   v_lineId[4];  // Line ID
     in vec3  v_pos[4];     // Position (world space)
     in vec3  v_latlon[4];  // Position (lat/lon/height)
     in float v_visible[4]; // Line visiblity
     in float v_color[4];   // Line color (0..1, mapped on color map)
     in float v_width[4];   // Line width (0..1)


////////////////////////////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
////////////////////////////////////////////////////////////////////////////////////////////////////

flat          out int   g_lineId;    // Line ID
noperspective out vec2  g_screenpos; // Position (screen space)
              out float g_width;     // Line width (0..1)
              out vec3  g_color;     // Line color (0..1, mapped on color map)


////////////////////////////////////////////////////////////////////////////////////////////////////
// Imported functions
////////////////////////////////////////////////////////////////////////////////////////////////////

vec4  mapColor(float v);
float pixelLength(vec4 clipStart, vec4 clipEnd);
vec2  clipToPixelSpace(vec4 clipCoords);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry shader
////////////////////////////////////////////////////////////////////////////////////////////////////

void main(void)
{
    // ignore line if it is marked disabled
    if (v_visible[0] * v_visible[1] * v_visible[2] * v_visible[3] == 0.0 ||
        v_lineId[0] != v_lineId[1] || v_lineId[1] != v_lineId[2] || v_lineId[2] != v_lineId[3])
    {
        return;
    }

    // compute number of parts - make sure it's even or 1
    vec4 clipPos1 = modelViewProjectionMatrix * vec4(v_pos[1], 1.0);
    vec4 clipPos2 = modelViewProjectionMatrix * vec4(v_pos[2], 1.0);
    float lineLength = pixelLength(clipPos1, clipPos2);
    int maxParts = (MAX_VERTICES-1) - (MAX_VERTICES-1) % 2;
    int numParts = clamp(2 * int(lineLength / 20), 1, maxParts);
    for (int i=0; i<=numParts; i++) {
        float t = float(i) / float(numParts);

        // interpolate position and attributes
        vec3  pos   = mix(v_pos   [1]  , v_pos   [2]  , t);
        float h     = mix(v_latlon[1].z, v_latlon[2].z, t);
        float width = mix(v_width [1]  , v_width [2]  , t);
        float color = mix(v_color [1]  , v_color [2]  , t);

        // correct height, add arc
//        pos = correctHeight(pos, h);

        // convert to clip position
        vec4 clipPos = modelViewProjectionMatrix * vec4(pos, 1.0);

        // adjust depth
//      clipPos.z = ((edgesOnTop ? 0.4 : 0.9) - 0.3 * 1.0) * clipPos.w;

        // set output variables
        gl_Position  = clipPos;
        g_lineId     = v_lineId[1];
        g_screenpos  = clipToPixelSpace(clipPos);
        g_width      = width;
        g_color      = mapColor(color).rgb;
        EmitVertex();
    }

    EndPrimitive();
}
