#version 330


////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////////////////////////////////////////////////////

// Visibility based on attribute filters
uniform samplerBuffer visibility;

// Temporal filters
uniform vec2 timeSpan1;
uniform vec2 timeSpan2;
uniform vec2 timeSpan3;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if vertex is visible or not (based on attribute filters for nodes)
float filterNodeAttrs(int nodeIndex)
{
    return texelFetch(visibility, nodeIndex).r;
}

// Check if vertex is visible or not (based on attribute filters for nodes and edges)
float filterAttrs(int nodeIndex, int edgeIndex)
{
    return texelFetch(visibility, nodeIndex).r * texelFetch(visibility, edgeIndex).r;
}

// Check if vertex is visible according to time span #1
// Returns 1.0 if visible, 0.0 if not
float filterTimespan1(float time)
{
    bool visible = (time >= timeSpan1.x && time <= timeSpan1.y) || time < 0.0;
    return (visible ? 1.0 : 0.0);
}

// Check if vertex is visible according to time span #2
// Returns 1.0 if visible, 0.0 if not
float filterTimespan2(float time)
{
    bool visible = (time >= timeSpan2.x && time <= timeSpan2.y) || time < 0.0;
    return (visible ? 1.0 : 0.0);
}

// Check if vertex is visible according to time span #3
// Returns 1.0 if visible, 0.0 if not
float filterTimespan3(float time)
{
    bool visible = (time >= timeSpan3.x && time <= timeSpan3.y) || time < 0.0;
    return (visible ? 1.0 : 0.0);
}
