#version 330 core
layout (location = 0) in vec3 vertexPos;

out vec3 worldPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	worldPos = vertexPos;
	gl_Position = projection * view * model * vec4(worldPos, 1.0);
}