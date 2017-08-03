#version 330 core

in vec3 outColor;

uniform vec3 lightBallColor;

out vec3 color;

void main()
{
	color = lightBallColor;
}