#version 450

layout(location = 2) uniform vec4 Color;

void main()
{
	gl_FragColor = Color;
}