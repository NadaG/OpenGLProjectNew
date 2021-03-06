#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexUV;

out vec3 worldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	worldPos = vertexPos;
	gl_Position = projection * view * vec4(worldPos, 1.0);
}