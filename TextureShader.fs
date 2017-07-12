#version 330 core

in vec3 outVertexColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

out vec3 color;

void main()
{
	color = texture(ourTexture, TexCoord).rgb;
}