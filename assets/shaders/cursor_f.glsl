#version 330

uniform sampler2D tex;

in vec2 Texcoord;

void main()
{
	gl_FragColor = texture(tex, Texcoord);
}
