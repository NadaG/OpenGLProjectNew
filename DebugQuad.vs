#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = vertexTexCoord;
	gl_Position = vec4(vertexPos, 1.0);
}