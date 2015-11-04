
#version 330


// Uniforms
uniform sampler1D colorMap;


// [TODO] Hard-coded color map function
vec4 mapColor(float v)
{
    vec4 c0 = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 c1 = vec4(1.0, 1.0, 1.0, 1.0);
    return c0 + (c1 - c0) * v;
}

/*
// Map value (0..1) to color, using the given color map texture
vec4 mapColor(float v)
{    
    return texture(colorMap, clamp(v, 0.0, 1.0));
}

vec4 mapToColor(in sampler1D map, float v)
{    
    return texture(map, clamp(v, 0.0, 1.0));
}
*/
