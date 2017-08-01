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

uniform vec3 lightPos;
uniform vec3 eyePos;

out vec3 tangentLightPos;
out vec3 tangentEyePos;
out vec3 tangentOutPos;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);
	outPos = vec3(model * vec4(vertexPos, 1));

	vec3 T = normalize(vec3(model * vec4(vertexTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(vertexNormal, 0.0)));

	// 왜 하는지는 아직 잘 모르겠음
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);

	mat3 TBN = transpose(mat3(T, B, N));
	outColor = vertexColor;
	outNormal = vertexNormal;
	outTexCoord = vertexTexCoord;

	tangentLightPos = TBN * lightPos;
	tangentEyePos = TBN * eyePos;
	tangentOutPos = TBN * outPos;
}