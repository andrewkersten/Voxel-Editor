#version 330

#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) uniform mat4 projection;
layout (location = 4) uniform mat4 modelview;

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;

out vec4 Color;

void main()
{
	Color = color;

	//gl_Position = projection * modelview * vec4(position, 1.0, 1.0);
	gl_Position = projection * vec4(position, 1.0, 1.0);
}
