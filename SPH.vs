#version 330 core

layout(location = 0) in vec2 position;

out vec3 outPos;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(position, 0.0, 1.0);
	outPos = vec3(position.x, position.y, 0.0);
}