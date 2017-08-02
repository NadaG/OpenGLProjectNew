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

	T = normalize(T);
	vec3 B = cross(N, T);

	// 각 열이 서로 직각을 이루는 행렬에 대해서는 직교행렬이라고 부르고 이 행렬은 inverse가 transpose와 같다
	// glsl에서는 matnxm이라고 하면 n열 m행으로 표시된다
	// mat3 matrix;
	// matrix[1] = vec3(3.0, 3.0, 3.0)을 하면 2번째 열의 모든 원소가 3.0이 된다.
	// matrix[2][0] 은 3번째 열의 1번 행 원소를 가르킨다.
	// glsl의 경우 column major이기 때문에 
	// mat3(T, B, N)을 할 경우 
	// Tx Bx Nx
	// Ty By Ny
	// Tz Bz Nz로 들어간다
	// mat4가 아니라 mat3인 이유는 tangent space에서는 direction만 다루기 때문에 position이 중요하지 않기 때문이다
	mat3 TBN = transpose(mat3(T, B, N));
	
	outColor = vertexColor;
	outNormal = N;
	outTexCoord = vertexTexCoord;

	// normal map에 있는 normal은 tangent space로 볼 수 있다.
	// 이 normal을 world space로 바꾸는 대신 빛에 관련된 요소들을 tangent space로 바꾸는 방법을 사용했다
	tangentLightPos = TBN * lightPos;
	tangentEyePos = TBN * eyePos;
	tangentOutPos = TBN * outPos;
}