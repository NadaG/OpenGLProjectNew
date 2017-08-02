#version 330 core

// location ���� glVertexAttribPointer�� ù��° ���ڿ� ���� ���ƾ� �Ѵ�
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

	// �� ���� ���� ������ �̷�� ��Ŀ� ���ؼ��� ��������̶�� �θ��� �� ����� inverse�� transpose�� ����
	// glsl������ matnxm�̶�� �ϸ� n�� m������ ǥ�õȴ�
	// mat3 matrix;
	// matrix[1] = vec3(3.0, 3.0, 3.0)�� �ϸ� 2��° ���� ��� ���Ұ� 3.0�� �ȴ�.
	// matrix[2][0] �� 3��° ���� 1�� �� ���Ҹ� ����Ų��.
	// glsl�� ��� column major�̱� ������ 
	// mat3(T, B, N)�� �� ��� 
	// Tx Bx Nx
	// Ty By Ny
	// Tz Bz Nz�� ����
	// mat4�� �ƴ϶� mat3�� ������ tangent space������ direction�� �ٷ�� ������ position�� �߿����� �ʱ� �����̴�
	mat3 TBN = transpose(mat3(T, B, N));
	
	outColor = vertexColor;
	outNormal = N;
	outTexCoord = vertexTexCoord;

	// normal map�� �ִ� normal�� tangent space�� �� �� �ִ�.
	// �� normal�� world space�� �ٲٴ� ��� ���� ���õ� ��ҵ��� tangent space�� �ٲٴ� ����� ����ߴ�
	tangentLightPos = TBN * lightPos;
	tangentEyePos = TBN * eyePos;
	tangentOutPos = TBN * outPos;
}