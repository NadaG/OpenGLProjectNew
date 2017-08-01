#version 330 core

// location 값은 glVertexAttribPointer의 첫번째 인자와 값이 같아야 한다
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;

out vec3 outPos;
out vec3 outNormal;
out vec2 outTexCoord;
out vec4 outFragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	// world position
	outPos = vec3(model * vec4(vertexPos, 1.0));
	outNormal = transpose(inverse(mat3(model))) * vertexNormal;
	outTexCoord = vertexTexCoord;
	// light space position
	outFragPosLightSpace = lightSpaceMatrix * vec4(outPos, 1.0);
	outFragPosLightSpace /= outFragPosLightSpace.w;
	// camera space position
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}