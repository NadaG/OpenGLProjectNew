#version 330 core

uniform vec3 lightPos;
uniform vec3 eyePos;

out vec3 color;
in vec3 outPos;
in vec3 outColor;
in vec3 outNormal;
in vec2 outTexCoord;

void main()
{
	vec3 lightDirection = -normalize(lightPos - outPos);

	vec3 ambient = vec3(0.1, 0.1, 0.1);
	vec3 diffuse = dot(-normalize(lightDirection), outNormal) * outColor;
	vec3 eyeDirection = normalize(outPos - eyePos);

	vec3 specular = pow(max(dot(-normalize(reflect(lightDirection, outNormal)), eyeDirection), 0.0), 64) * outColor;

	color = ambient + diffuse + specular;
}