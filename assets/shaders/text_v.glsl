#version 330

#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) uniform mat4 projection;

layout (location = 0) in vec4 coords;

out vec2 texcoord;

void main()
{
	gl_Position = projection * vec4(coords.xy, 0.0, 1.0);
	texcoord = coords.zw;
}
