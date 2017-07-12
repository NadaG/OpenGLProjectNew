#version 330 core

// location 값은 glVertexAttribPointer의 첫번째 인자와 값이 같아야 한다
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;

out vec3 outPos;
out vec3 outColor;
out vec3 outNormal;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);

	vec4 Position = vec4(vertexPos, 1);

	outColor = vertexColor;
	outNormal = vertexNormal;
	outPos = Position.xyz;
}