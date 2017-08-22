#version 330 core

// location ���� glVertexAttribPointer�� ù��° ���ڿ� ���� ���ƾ� �Ѵ�
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexTexCoord;

out vec2 outTexCoord;

void main()
{
	outTexCoord = vertexTexCoord;
	gl_Position = vec4(vertexPos, 1.0);
}