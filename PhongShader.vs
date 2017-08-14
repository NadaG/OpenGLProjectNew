#version 330 core

// location 값은 glVertexAttribPointer의 첫번째 인자와 값이 같아야 한다
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

uniform mat4 MVP;
uniform mat4 model;

out vec3 outPos;
out vec3 outNormal;
out vec2 outTexCoord;

void main()
{
	vec4 worldPos = model * vec4(vertexPos, 1.0);
	outPos = worldPos.xyz;
	outTexCoord = vertexTexCoord;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	outNormal = normalMatrix * vertexNormal;

	gl_Position = MVP * vec4(vertexPos, 1.0);
}