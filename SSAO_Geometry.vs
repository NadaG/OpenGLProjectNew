#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

out vec3 outPos;
out vec2 outTexCoord;
out vec3 outNormal;

uniform bool invertedNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 viewPos = view * model * vec4(vertexPos, 1.0);
	outPos = viewPos.xyz;
	outTexCoord = vertexTexCoord;

	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	outNormal = normalMatrix * (invertedNormals ? -vertexNormal : vertexNormal);

	gl_Position = projection * viewPos;
}