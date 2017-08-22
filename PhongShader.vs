#version 330 core

// location 값은 glVertexAttribPointer의 첫번째 인자와 값이 같아야 한다
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexTexCoord;

out vec2 outTexCoord;

void main()
{
	outTexCoord = vertexTexCoord;
	gl_Position = vec4(vertexPos, 1.0);
}