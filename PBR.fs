#version 330 core
out vec3 color;
in vec2 outTexCoord;
in vec3 worldPos;
in vec3 outNormal;

uniform vec3 eyePos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

uniform float ao;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

const float PI = 3.141592;

// 역시 인자로 넘어온 것은 world normal
// model pos에 있던 것을 model matrix(world에서 바라 본 model에 달린 좌표계)를 곱함으로써 world pos로 가져왔듯이
// tangent normal에 있던 것을 tbn matrix(world에서 바라 본 tangent에 달린 좌표계)를 곱함으로써 world normal로 가져왔다.
vec3 getNormalFromMap()
{
	// tangent space 상에서의 normal, 거의 파란색임
	vec3 tangentNormal = texture(normalMap, outTexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(worldPos);
    vec3 Q2  = dFdy(worldPos);
    vec2 st1 = dFdx(outTexCoord);
    vec2 st2 = dFdy(outTexCoord);

    vec3 N   = normalize(outNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// F function
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// D function
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	// numerator(nominator) 분자, denominator 분모
	float nom = a2;
	float denom = (NdotH2*(a2-1.0)+1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

// G function
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main()
{
	vec3 N = getNormalFromMap();
	vec3 V = normalize(eyePos - worldPos);

	vec3 Lo = vec3(0.0);
	vec3 albedo = texture(albedoMap, outTexCoord).rgb;
	albedo = vec3(0.8, 0.8, 0.8);
	float metallic = texture(metallicMap, outTexCoord).r;
	float roughness = texture(roughnessMap, outTexCoord).r;

	for(int i = 0; i < 4; i++)
	{
		vec3 L = normalize(lightPositions[i] - worldPos);
		vec3 H = normalize(V + L);

		float distance = length(lightPositions[i] - worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		vec3 F0 = vec3(0.04);
		// (x, y, a) => x*(1-a) + a*y
		F0 = mix(F0, albedo, metallic);
		
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
				
		// Cook-Torrance BRDF
		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = nominator / denominator;
		
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.5) * albedo;
	color = ambient + Lo;

	// HDR tonemapping
	// color = color / (color * vec3(1.0));
	// gamma correction
	// color = pow(color, vec3(1.0/2.2));
}