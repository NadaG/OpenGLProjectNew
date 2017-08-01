#version 330 core

out vec3 color;

in vec3 outPos;
in vec3 outNormal;
in vec2 outTexCoord;
in vec4 outFragPosLightSpace;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 eyePos;

// OpenGL에서는 x,y,z를 모두 [-1, 1]로 하는 것이 NDC이다

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// w를 나눔으로써 [-w, w]의 공간을 [-1, 1]로 바꾼다
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// depthmap에 있는 값은 [0, 1]이므로 이것과 맞추기 위해 곱하기 0.5와 0.5를 더한다
	projCoords = projCoords * 0.5 + 0.5;
	// NDC에서 xy를 가져옴으로써 depthmap texture에 대응하는 depth값을 가져옴
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
	// abmient는 그냥 정해짐
	// diffuse는 lightColor와 (normal과 lightDir)으로 정해짐
	//  
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

	color = lighting;
}