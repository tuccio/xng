#version 450

layout(location = 0) uniform mat4 ModelViewMatrix;
layout(location = 1) uniform mat4 ProjectionMatrix;

layout(location = 0) in vec3 position;

void main()
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(position, 1);
}