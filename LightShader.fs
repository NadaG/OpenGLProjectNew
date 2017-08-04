#version 330 core

in vec3 outColor;

uniform vec3 lightBallColor;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 brightColor;

void main()
{
	color = lightBallColor;
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = color;
}