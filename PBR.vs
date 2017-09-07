#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec2 vertexTexCoord;
layout (location = 4) in vec3 vertexTangent;

out vec2 outTexCoord;
out vec3 worldPos;
out vec3 outNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	outTexCoord = vertexTexCoord;
	worldPos = vec3(model * vec4(vertexPos, 1.0));
	outNormal = mat3(model) * vertexNormal;

	gl_Position = projection * view * vec4(worldPos, 1.0);
}