#version 330

//#extension GL_ARB_explicit_uniform_location : enable

const vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

in vec2 texcoord;

//layout (location = 5) uniform vec4 color;

uniform sampler2D tex;

void main()
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(tex, texcoord).r) * color;
}
