#version 330 core

layout (location = 0) out vec3 color;
layout (location = 1) out vec3 brightColor;

in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
} fs_in;

struct Light {
    vec3 position;
    vec3 color;
};

uniform vec3 lightPos;
uniform Light lights[16];
uniform sampler2D diffuseTexture;

void main()
{
	brightColor = vec3(0.0);
    vec3 texColor = texture(diffuseTexture, fs_in.texCoord).rgb;
    vec3 normal = normalize(fs_in.normal);
    // ambient
    vec3 ambient = 0.0 * texColor;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 16; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lightPos - fs_in.pos);
        float diff = max(dot(lightDir, normal), 0.0);
		// diff = 1.0;
        vec3 diffuse = lights[i].color * diff * texColor;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.pos - lightPos);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }
    color = ambient + lighting;
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = color;
}