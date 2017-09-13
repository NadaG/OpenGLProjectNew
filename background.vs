#version 330 core
layout (location = 0) in vec3 vertexPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 worldPos;

void main()
{
	worldPos = vertexPos;
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projecton * rotView * vec4(worldPos, 1.0);

	gl_Position = clipPos.xyww;
}