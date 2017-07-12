#version 330 core

uniform vec3 lightDirection;
uniform vec3 eyePos;

out vec3 color;
in vec3 outPos;
in vec3 outColor;
in vec3 outNormal;

void main()
{
	vec3 diffuse = dot(-normalize(lightDirection), outNormal) * outColor;
	vec3 eyeDirection = normalize(outPos - eyePos);

	vec3 specular = pow(max(dot(-normalize(reflect(lightDirection, outNormal)), eyeDirection), 0.0), 32) * outColor;

	color = diffuse + specular;
}