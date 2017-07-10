#version 330 core

in vec3 outVertexColor;

out vec3 color;

void main()
{
	color = outVertexColor;	
}