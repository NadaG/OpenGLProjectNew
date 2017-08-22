#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 outTexCoord;
in vec3 outPos;
in vec3 outNormal;

uniform sampler2D diffuseMap;

void main()
{    
    gPosition = outPos;
    gNormal = normalize(outNormal);
	
	gAlbedo.rgb = texture(diffuseMap, outTexCoord).rgb;
    //gAlbedo.rgb = vec3(0.95);
}