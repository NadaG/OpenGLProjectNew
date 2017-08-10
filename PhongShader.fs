#version 330 core

out vec3 color;
in vec3 outPos;
in vec3 outColor;
in vec3 outNormal;
in vec2 outTexCoord;

in vec3 tangentLightPos;
in vec3 tangentEyePos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{
	vec3 norm = texture(normalMap, outTexCoord).xyz;
	norm = norm * 2 - 1.0;
	vec3 lightDirection = -normalize(tangentLightPos - outPos);

	vec3 texColor = texture(diffuseMap, outTexCoord).rgb;
	vec3 ambient = 0.2 * texColor;
	vec3 diffuse = max(0.0, dot(-normalize(lightDirection), norm)) * texColor;
	vec3 eyeDirection = normalize(outPos - tangentEyePos);

	vec3 specular = pow(max(dot(-normalize(reflect(lightDirection, norm)), eyeDirection), 0.0), 64) * texColor;

	color = ambient + diffuse + specular;
}