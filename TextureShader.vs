#version 330 core

// location ���� glVertexAttribPointer�� ù��° ���ڿ� ���� ���ƾ� �Ѵ�
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 MVP;

out vec2 TexCoord;
out vec3 outVertexColor;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);
	outVertexColor = vertexColor;
	TexCoord = aTexCoord;
}