#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 outTexCoord;
in vec3 outPos;
in vec3 outNormal;

void main()
{    
    gPosition = outPos;
    gNormal = normalize(outNormal);
	
	//gAlbedo.rgb = texture(diffuseMap, outTexCoord).rgb;
	gAlbedo.rgb = vec3(1.0, 1.0, 1.0);
}