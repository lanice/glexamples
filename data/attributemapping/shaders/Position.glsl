
#version 330


// Function to transform an input vector into a 3D-position
vec4 calculatePosition(vec3 inVector)
{
    return vec4(inVector, 1.0);
}
