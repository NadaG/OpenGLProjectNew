#version 330 core
out vec3 color;
in vec2 outTexCoord;
in vec3 worldPos;
in vec3 outNormal;

uniform vec3 camPos;
uniform vec3 albedo;

uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 N = normalize(outNormal);
	vec3 V = normalize(camPos - worldPos);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; i++)
	{
		vec3 L = normalize(lightPositions[i] - worldPos);
		vec3 H = normalize(V + L);

		float distance = length(lightPositions[i] - worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		vec3 F0 = vec3(0.04);
		// (x, y, a) => x*(1-a) + a*y
		F0 = mix(F0, albedo, metallic);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
	}

	color = vec3(0.0, 0.0, 1.0);
}