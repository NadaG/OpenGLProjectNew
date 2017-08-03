#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec2 texCoord;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vs_out.pos = vec3(model * vec4(vertexPos, 1.0));
	vs_out.texCoord = vertexTexCoord;

	vec3 n = -vertexNormal;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.normal = normalize(normalMatrix * n);
	
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}