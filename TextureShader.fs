#version 330 core

in vec3 outPos;
in vec3 outColor;
in vec3 outNormal;
in vec2 outTexCoord;

in vec3 tangentLightPos;
in vec3 tangentEyePos;
in vec3 tangentOutPos;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform Material material;
uniform sampler2D normalMap;

out vec3 color;

void main()
{
	vec3 ambient = lightColor * vec3(texture(material.diffuse, outTexCoord));
	
	vec3 norm = texture(normalMap, outTexCoord).rgb;
	// [0, 1]가 아니라 [-1, 1]로 되어야 하므로, (하므로와 함으로가 헛갈린다면 '하기 때문에'로 바꿔서 말이 되면 '하므로')
	norm = normalize(norm * 2.0 - 1.0);
	vec3 lightDir = normalize(tangentLightPos - tangentOutPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * diff * vec3(texture(material.diffuse, outTexCoord));

	vec3 eyeDir = normalize(tangentEyePos - tangentOutPos);
	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	float spec = pow(max(dot(eyeDir, reflectDir), 0.0), material.shininess);
	//vec3 specular = lightColor * spec * vec3(texture(material.specular, outTexCoord));
	vec3 specular = lightColor * spec;

	color = ambient + diffuse + specular;
}