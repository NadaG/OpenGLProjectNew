#version 330 core
out vec3 color;
in vec3 worldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265459;

void main()
{
	// world pos�� �������� ������ normal�̶�� �ΰ� �̰��� �̤ˤ����ؼ� irradiance�� ������
	vec3 N = normalize(worldPos);

	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, N);
	up = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
			// tangent space to world space�̴�.
			// T B N ��� * tangentSampleVec �����̶�� ���� �ȴ�.
			vec3 sampleVec = tangentSample.x*right + tangentSample.y*up + tangentSample.z*N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0/float(nrSamples));

	color = irradiance;
}