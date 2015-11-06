
#version 330


////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////////////////////////////////////////////////////

// Textures
uniform sampler2DArray textures;
uniform sampler2DArray colorMaps;

// Blending mode
uniform int sourceBlendMode = 6;
uniform int destinationBlendMode = 7;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Input from geometry shader
////////////////////////////////////////////////////////////////////////////////////////////////////

// Global
flat in float g_textureID;
flat in float g_colorMapID;
flat in float g_geometryType;
flat in float g_alpha;
flat in float g_lineId;
     in float g_color;

// For tubes
in vec2  g_texcoord;

// For spheres
in vec3  g_pos;
in vec3  g_center;
in vec3  g_up;
in float g_radius;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Output to frame buffer
////////////////////////////////////////////////////////////////////////////////////////////////////

layout (location = 0) out vec4  outputColor;
layout (location = 1) out float outputId;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry #1: Tubes
////////////////////////////////////////////////////////////////////////////////////////////////////

// Forward declarations
vec4 blendFunction(in int modeSource, in int modeDestination, in vec4 source, in vec4 destination);

void renderTube()
{
    vec4 color = vec4(texture(colorMaps, vec3(g_color, 0, g_colorMapID)).rgb, g_alpha);

    outputColor = blendFunction(
        sourceBlendMode,
        destinationBlendMode,
        texture(textures, vec3(g_texcoord.x, g_texcoord.y, g_textureID)),
        color);

    outputId = g_lineId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry #2: Spheres
////////////////////////////////////////////////////////////////////////////////////////////////////

vec4 lightingAniso(vec3 position, vec3 tangent, vec3 normal)
{
    // Light source definition
    vec3 lightPos  = vec3(0.0, 0.0, 0.0);
    vec4 lightDiff = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 lightSpec = vec4(1.0, 1.0, 1.0, 1.0);

    // Material definition
    vec4  diffuse   = vec4(1.0, 1.0, 1.0, 1.0);
    vec4  specular  = vec4(0.3, 0.3, 0.3, 1.0);
    float shininess = 4.0f;

    // Normalize input
    vec3 T = normalize(tangent);
    vec3 N = normalize(normal);
    vec3 V = position;

    // Compute vectors
    vec3 L = normalize(lightPos - V);
    vec3 H = normalize(L - normalize(V.xyz));
    vec3 R = reflect(-L, N);
    vec3 E = vec3(0.0, 0.0, 1.0);

    // Diffuse
    float LdotT = dot(L, T);
    float VdotT = dot(normalize(V), T);
    float Idiff = sqrt(1.0-LdotT*LdotT);

    // Specular
    float Ispec = pow(max(0.0, dot(R, E)), shininess);

    // Calculate color
    float self = max(0.0, dot(N, L));
    vec4 color = texture(colorMaps, vec3(g_color, 0, g_colorMapID));
    vec4 res   = self * color * (diffuse*lightDiff*Idiff + specular*lightSpec*Ispec);

    // Preserve transparency, but make sure that all pixels of the sphere have the same transparency value
    res.a = color.a;
    return res;
}

void renderSphere()
{
    // Ray tracing center -> pos
    vec3 origin = vec3(0.0, 0.0, 0.0);
    vec3 dst = origin - g_center;
    vec3 dir = normalize(g_pos - origin);
    float r = 0.0;
    float B = dot(dst, dir);
    float C = dot(dst, dst) - g_radius*g_radius;
    float D = B*B - C;
    if (D<0.0) discard;
    if (D==0.0) {
        r = -B;
    } else {
        float Ds = sqrt(D);
        float t =  -B - Ds;
        if (t>0.0) {
            r = t;
        } else {
            t =  -B + Ds;
            if (t>0.0) {
                r = t;
            } else {
                discard;
            }
        }
    }

    vec3 pK      = r * dir;
    vec3 normal  = pK - g_center;
    vec3 tangent = cross(normal, g_up);

    outputColor  = vec4(lightingAniso(pK, tangent, normal)); //.rgb, 0.2);
    outputId     = g_lineId;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////

void main()
{
    outputId = 0.0;
    if (g_geometryType >= 2.0)
        renderSphere();
    else
        renderTube();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Code below kindly provided by FNC: Library for Focus + Context Visualization
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Blending function
//
vec4 performBlending(in vec4 CS, in vec4 CD, in vec4 S, in vec4 D)
{
    return vec4(CS.r*S.r+CD.r*D.r, CS.g*S.g+CD.g*D.g, CS.b*S.b+CD.b*D.b, CS.a*S.a+CD.a*D.a);
}

//
// Supported
//
#define BLEND_ZERO                         0
#define BLEND_ONE                          1
#define BLEND_DST_COLOR                    2
#define BLEND_SRC_COLOR                    3
#define BLEND_ONE_MINUS_DST_COLOR          4
#define BLEND_ONE_MINUS_SRC_COLOR          5
#define BLEND_SRC_ALPHA                    6
#define BLEND_ONE_MINUS_SRC_ALPHA          7
#define BLEND_DST_ALPHA                    8
#define BLEND_ONE_MINUS_DST_ALPHA          9

//
// Map blend factors ... no switch statement...too bad :|
//
vec4 mapBlendFactor(in int modeSource, in vec4 source, in vec4 destination)
{
    switch (modeSource)
    {
        case BLEND_ZERO:                  return vec4(0.0);
        case BLEND_ONE:                   return vec4(1.0);
        case BLEND_DST_COLOR:             return destination;
        case BLEND_SRC_COLOR:             return source;
        case BLEND_ONE_MINUS_DST_COLOR:   return vec4(1.0) - destination;
        case BLEND_ONE_MINUS_SRC_COLOR:   return vec4(1.0) - source;
        case BLEND_SRC_ALPHA:             return vec4(source.a);
        case BLEND_ONE_MINUS_SRC_ALPHA:   return vec4(1.0) - vec4(source.a);
        case BLEND_DST_ALPHA:             return vec4(destination.a);
        case BLEND_ONE_MINUS_DST_ALPHA:   return vec4(1.0) - vec4(destination.a);
        default:                          return vec4(1.0);
    }
}

//
// Put it together
//
vec4 blendFunction(
    in int modeSource,
    in int modeDestination,
    in vec4 source,
    in vec4 destination)
{
    return performBlending(
        mapBlendFactor(modeSource, source, destination),
        mapBlendFactor(modeDestination, source, destination),
        source,
        destination);
}
