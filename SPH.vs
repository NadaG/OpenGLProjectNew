#version 330 core

layout(location = 0) in vec2 position;

out vec3 outPos;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
	outPos = vec3(position.x, position.y, 0.0);
}