#version 140
#extension GL_ARB_explicit_attrib_location : require

uniform float elapsed;

layout (location = 0) out vec4 fragColor;
in vec2 v_uv;

void main()
{
	fragColor = vec4(vec3(16.0 * elapsed), 0.0);
}
