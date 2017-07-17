#version 330 core

in vec3 TexCoord;

uniform samplerCube skybox;

out vec3 color;

void main()
{
	color = vec3(texture(skybox, TexCoord));
}