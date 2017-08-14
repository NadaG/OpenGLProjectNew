#version 330 core

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragDiffuse;

in vec3 outPos;
in vec3 outNormal;
in vec2 outTexCoord;

uniform sampler2D diffuseMap;

void main()
{
	fragPos = outPos;
	fragNormal = normalize(outNormal);
	fragDiffuse = texture(diffuseMap, outTexCoord).rgb;
}