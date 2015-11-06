#version 330

#define NUM_ATTRS 32


////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////////////////////////////////////////////////////

// Attribute data
uniform samplerBuffer attributes;
uniform int numNodeAttributes = 0;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Read minimum value of attribute <attr>
float minAttributeValue(int attr)
{
    if (attr >= 0) {
        return texelFetch(attributes, (0) * NUM_ATTRS + attr).r;
    } else {
        return 0.0;
    }
}

// Read maximum value of attribute <attr>
float maxAttributeValue(int attr)
{
    if (attr >= 0) {
        return texelFetch(attributes, (1) * NUM_ATTRS + attr).r;
    } else {
        return 0.0;
    }
}

// Read attribute <attr> of vertex <index> from texture data (unmodified value)
float attributeValue(int nodeIndex, int edgeIndex, int attr)
{
    int index = (attr < numNodeAttributes ? nodeIndex : edgeIndex);

    if (attr >= 0) {
        // Get attribute value
        return texelFetch(attributes, (index + 4) * NUM_ATTRS + attr).r;
    } else {
        return 0.0;
    }
}

// Read attribute <attr> of vertex <index> from texture data (normalized between 0.0 and 1.0)
// Linear normalization
float normalizedAttributeLinear(int nodeIndex, int edgeIndex, int attr)
{
    if (attr >= 0) {
        int index = (attr < numNodeAttributes ? nodeIndex : edgeIndex);

        // Get attribute value
        float value = texelFetch(attributes, (index + 4) * NUM_ATTRS + attr).r;
        float min   = texelFetch(attributes, (        0) * NUM_ATTRS + attr).r;
        float max   = texelFetch(attributes, (        1) * NUM_ATTRS + attr).r;
        float mean  = texelFetch(attributes, (        2) * NUM_ATTRS + attr).r;
        float sigma = texelFetch(attributes, (        3) * NUM_ATTRS + attr).r;

        return (value - min) / (max - min);
    } else {
        return 0.0;
    }
}

// Read attribute <attr> of vertex <index> from texture data (normalized between 0.0 and 1.0)
// n-sigma normalization
float normalizedAttribute(int nodeIndex, int edgeIndex, int attr)
{
    if (attr >= 0) {
        int index = (attr < numNodeAttributes ? nodeIndex : edgeIndex);

        // Get attribute value
        float value = texelFetch(attributes, (index + 4) * NUM_ATTRS + attr).r;
        float min   = texelFetch(attributes, (        0) * NUM_ATTRS + attr).r;
        float max   = texelFetch(attributes, (        1) * NUM_ATTRS + attr).r;
        float mean  = texelFetch(attributes, (        2) * NUM_ATTRS + attr).r;
        float sigma = texelFetch(attributes, (        3) * NUM_ATTRS + attr).r;

        // Calculate n-sigma range
        float n = 1;
        min = mean - n * sigma;
        max = mean + n * sigma;

        // Calculate value
        if (abs(max - min) < 0.00001) {
            return clamp(value, 0.0, 1.0);
        } else {
            return (value - min) / (max - min);
        }
    } else {
        return 0.0;
    }
}
