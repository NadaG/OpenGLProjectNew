#version 330 core

out vec3 color;

in vec2 outTexCoord;

uniform sampler2D screenTexture;

// gray scale = rgb 평균값으로 rgb를 채우는 방식
// 사람의 눈은 green에 민감하고 blue에 둔감하기 때문에 0.2126, 0.7152, 0.0722(합이 1)등을 곱해줌
//void main()
//{
//	color = texture(screenTexture, outTexCoord).rgb;
//	float average = 0.2126*color.r + 0.7152*color.g + 0.0722*color.b;
//	color = vec3(average, average, average);
//}

const float offset = 1.0 / 300.0;

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),  // top-left
		vec2(0.0f, offset),     // top-center
		vec2(offset, offset),   // top-right
		vec2(-offset, offset),  // center-left
		vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right 
	);

	float kernel[9] = float[](
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	);

	// 마지막에 컬러 값을 정할 때 모든 원소들의 합인 16으로 나눌 것
	float blurKernel[9] = float[](
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	);

	float edgeDetection[9] = float[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(screenTexture, outTexCoord + offsets[i]));
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * blurKernel[i] / 16;

	color = col;
}