#version 330 core
out vec4 color;
in vec3 worldPos;

uniform samplerCube environmentMap;

void main()
{
	vec3 envColor = texture(environmentMap, worldPos).rgb;

	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0/2.2));

	color = envColor;
}