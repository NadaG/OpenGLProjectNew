#version 330 core

// location ���� glVertexAttribPointer�� ù��° ���ڿ� ���� ���ƾ� �Ѵ�
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;

uniform mat4 MVP;
uniform mat4 model;

out vec3 outPos;
out vec3 outColor;
out vec3 outNormal;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);

	outColor = vertexColor;
	outNormal = vertexNormal;
	outPos = vec3(model * vec4(vertexPos, 1));
}