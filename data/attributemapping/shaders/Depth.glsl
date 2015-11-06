#version 330


uniform mat4 projectionMatrix;


// Get linear depth (0..1) from position (must already have been transformed using ModelViewMatrix)
float getLinearDepth(vec4 pos)
{
    float dist  = -pos.z; 
    float A     =  projectionMatrix[2].z;
    float B     =  projectionMatrix[3].z;   
    float zFar  =  B / (1.0 + A);
    float zNear = -B / (1.0 - A);
    return (dist - zNear) / (zFar - zNear);
}
