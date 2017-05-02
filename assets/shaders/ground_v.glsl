#version 330

#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) uniform mat4 projection;
layout (location = 4) uniform mat4 modelview;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 Texcoord;

void main()
{
	Texcoord = texcoord;

	gl_Position = projection * modelview * vec4(position, 1.0);
}
