#version 330 core

out vec3 color;

in vec3 outPos;
in vec3 outNormal;
in vec2 outTexCoord;
in vec4 outFragPosLightSpace;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 eyePos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	vec3 lightColor = vec3(0.3);
	vec3 ambient = vec3(0.2);
	vec3 lightDir = normalize(lightPos - outPos);
	float diff = max(dot(lightDir, outNormal), 0.0);

	vec3 diffuse = diff * lightColor;
	vec3 viewDir = normalize(eyePos - outPos);

	color = ambient + diffuse;
}