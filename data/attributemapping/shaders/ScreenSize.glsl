
#version 330


// Uniforms
uniform vec2 screenSize;


// Compute pixel position on screen
vec2 clipToPixelSpace(vec4 clipCoords)
{
    vec2 posFactor = 0.5 * screenSize;
    return vec2(clipCoords.xy / clipCoords.w) * posFactor + posFactor;
}

// Compute distance between two clip coordinates in pixels
float pixelLength(vec4 clipStart, vec4 clipEnd)
{
    vec2 posFactor = 0.5 * screenSize;
    return length((clipStart.xy / clipStart.w - clipEnd.xy / clipEnd.w) * posFactor);
}

