#version 330 core

in vec3 outPos;
in vec3 outColor;
in vec3 outNormal;
in vec2 outTexCoord;

struct Material
{
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};

uniform sampler2D ourTexture;
uniform vec3 eyePos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform Material material;

out vec3 color;

void main()
{
	vec3 ambient = lightColor * vec3(texture(material.diffuse, outTexCoord));
	
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(lightPos - outPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * diff * vec3(texture(material.diffuse, outTexCoord));

	vec3 eyeDir = normalize(eyePos - outPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(eyeDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * spec * material.specular;

	color = ambient + diffuse + specular;
}