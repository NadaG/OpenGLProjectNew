#version 330 core

// location 값은 glVertexAttribPointer의 첫번째 인자와 값이 같아야 한다
layout(location = 0) in vec3 vertexPos;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);
}