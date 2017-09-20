#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexUV;

uniform mat4 projection;
uniform mat4 view;

out vec3 worldPos;

void main()
{
    worldPos = vertexPos;
	
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(worldPos, 1.0);

	gl_Position = clipPos.xyww;
}