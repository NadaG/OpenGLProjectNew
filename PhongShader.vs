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
out vec3 outColor;
out vec3 outNormal;
out vec2 outTexCoord;

out vec3 tangentLightPos;
out vec3 tangentEyePos;

uniform vec3 lightPos;
uniform vec3 eyePos;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);

	outColor = vertexColor;
	outNormal = vertexNormal;
	outPos = vec3(model * vec4(vertexPos, 1));
	outTexCoord = vertexTexCoord;

	vec3 T = normalize(vec3(model * vec4(vertexTangent, 1.0)));
	vec3 N = normalize(vec3(model * vec4(vertexNormal, 1.0)));

	vec3 B = normalize(cross(T, N));

	mat3 TBN = transpose(mat3(T, B, N));

	tangentLightPos = TBN * lightPos;
	tangentEyePos = TBN * eyePos;
}