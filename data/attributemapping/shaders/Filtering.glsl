
#version 330


////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if vertex is visible or not (based on attribute filters for nodes)
float filterNodeAttrs(int nodeIndex)
{
    return 1.0;
}

// Check if vertex is visible or not (based on attribute filters for nodes and edges)
float filterAttrs(int nodeIndex, int edgeIndex)
{
    return 1.0;
}

// Check if vertex is visible according to time span #1
// Returns 1.0 if visible, 0.0 if not
float filterTimespan1(float time)
{
    return 1.0;
}

// Check if vertex is visible according to time span #2
// Returns 1.0 if visible, 0.0 if not
float filterTimespan2(float time)
{
    return 1.0;
}

// Check if vertex is visible according to time span #3
// Returns 1.0 if visible, 0.0 if not
float filterTimespan3(float time)
{
    return 1.0;
}
