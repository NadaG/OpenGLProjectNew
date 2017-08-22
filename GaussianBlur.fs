#version 330 core

layout(location = 0) out vec3 color;

in vec2 outTexCoord;

uniform sampler2D image;

uniform bool horizontal;
// 종모양이고 noramlize가 되어서 적분한 값이 1이기 때문에 따로 평균을 낼 필요가 없다
float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 texOffset = 1.0 / textureSize(image, 0);
	vec3 result = texture(image, outTexCoord).rgb * weight[0];

	if(horizontal)
	{
		for(int i = 1; i < 5; i++)
		{
			result += texture(image, outTexCoord + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, outTexCoord - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; i++)
		{
			result += texture(image, outTexCoord + vec2(0.0, texOffset.y * i)).rgb * weight[i];
			result += texture(image, outTexCoord - vec2(0.0, texOffset.y * i)).rgb * weight[i];
		}
	}
	
	color = result;
}