
#version 330


// Uniforms
uniform sampler2D colorMap;


// Map value (0..1) to color, using the given color map texture
vec4 mapColor(float v)
{    
    return texture(colorMap, vec2(clamp(v, 0.0, 1.0), 0.0));
}

vec4 mapToColor(in sampler2D map, float v)
{    
    return texture(map, vec2(clamp(v, 0.0, 1.0), 0.0));
}
