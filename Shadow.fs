#version 330 core

out vec3 color;

in vec3 outPos;
in vec3 outNormal;
in vec2 outTexCoord;
in vec4 outFragPosLightSpace;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 eyePos;

// OpenGL������ x,y,z�� ��� [-1, 1]�� �ϴ� ���� NDC�̴�

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// w�� �������ν� [-w, w]�� ������ [-1, 1]�� �ٲ۴�
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// depthmap�� �ִ� ���� [0, 1]�̹Ƿ� �̰Ͱ� ���߱� ���� ���ϱ� 0.5�� 0.5�� ���Ѵ�
	projCoords = projCoords * 0.5 + 0.5;
	// NDC���� xy�� ���������ν� depthmap texture�� �����ϴ� depth���� ������
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	vec3 normal = normalize(outNormal);
	vec3 lightDir = normalize(lightPos - outPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	
	bias = 0.05;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main()
{
	vec3 normal = normalize(outNormal);
	vec3 lightColor = vec3(0.3);
	vec3 ambient = vec3(0.2);

	vec3 lightDir = normalize(lightPos - outPos);
	float diff = max(dot(lightDir, outNormal), 0.0);
	vec3 diffuse = diff * lightColor;
	vec3 viewDir = normalize(eyePos - outPos);

	float spec = 0.0;

	vec3 reflectDir = normalize(reflect(-lightDir, normal));
	spec = pow(max(dot(normal, reflectDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	float shadow = ShadowCalculation(outFragPosLightSpace);
	// abmient�� �׳� ������
	// diffuse�� lightColor�� (normal�� lightDir)���� ������
	//  
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

	color = lighting;
}