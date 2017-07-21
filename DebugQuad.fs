#version 330 core

out vec3 color;

in vec2 TexCoord;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

void main()
{
	float depthValue = texture(depthMap, TexCoord).r;
	color = vec3(depthValue);
}