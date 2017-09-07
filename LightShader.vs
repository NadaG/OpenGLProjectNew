#version 330 core

// location 값은 glVertexAttribPointer의 첫번째 인자와 값이 같아야 한다
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 outPos;
out vec3 outColor;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1);
	outColor = vertexColor;
}