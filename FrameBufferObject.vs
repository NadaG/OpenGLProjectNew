#version 330 core

layout (location = 0) in vec2 pixelPos;
layout (location = 1) in vec2 pixelTexCoord;

out vec2 outTexCoord;

void main()
{
	gl_Position = vec4(pixelPos.x, pixelPos.y, 0.0, 1.0);
	outTexCoord = pixelTexCoord;	
}