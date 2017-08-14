#version 330 core
out vec3 color;

in vec2 outTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuseMap;

struct Light
{
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
	float radius;
};

const int NR_LIGHTS = 32;

uniform Light lights[NR_LIGHTS];
uniform vec3 eyePos;

void main()
{
	vec3 FragPos = texture(gPosition, outTexCoord).rgb;
	vec3 Normal = texture(gNormal, outTexCoord).rgb;
	vec3 Albedo = texture(gDiffuseMap, outTexCoord).rgb;

	// output color
	vec3 lighting = Albedo * 0.1;
	vec3 eyeDir = normalize(eyePos - FragPos);
	for(int i = 0; i < NR_LIGHTS; i++)
	{
		vec3 lightDir = normalize(lights[i].position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].color;
		lighting += diffuse;
		// lighting += lights[i].position;
	}

	color = lighting;
	//color = vec3(1.0, 0.0, 0.0);
}