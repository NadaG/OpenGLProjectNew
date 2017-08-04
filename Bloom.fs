#version 330 core
out vec3 color;

in vec2 outTexCoord;

uniform sampler2D brightBlur;
uniform sampler2D scene;

uniform bool bloom;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, outTexCoord).rgb;
	vec3 bloomColor = texture(brightBlur, outTexCoord).rgb;

	if(bloom)
		hdrColor += bloomColor;

    // reinhard
    // vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
	// exp(x)는 e^x를 리턴한다
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    color = result;
}