#version 450 core
#define INV_PI 0.318309887
#define MAX_LIGHT_COUNT 40
#define LIGHT_WIDTH 0.5

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec2 uv;

struct Light{
	vec3 lightPos;
	vec3 lightColor;
	float radius;
};

uniform vec3 viewPos;
uniform vec2 lightNearAndFar;
uniform Light mainLight;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform samplerCube texture_shadow;
uniform sampler2D ambientOcclusion;
uniform sampler2D screenSpaceReflection;
uniform sampler2D pointLight;

const vec3 sampleOffsetDirection[27] = vec3[]
(
	vec3( 0,  0,  0), vec3( 0, -1,  0), vec3( 0,  1,  0), vec3( 0,   0, -1), 
	vec3( 0,  0,  1), vec3( 0, -1, -1), vec3( 0, -1,  1), vec3( 0,   1, -1), vec3( 0, 1, 1),
	vec3(-1,  0,  0), vec3(-1, -1,  0), vec3(-1, -1,  1), vec3(-1,	-1,  -1),
	vec3(-1,  1,  0), vec3(-1,  1,  1), vec3(-1,  1, -1), vec3(-1,   0,  -1), vec3(-1, 0, 1),
	vec3( 1,  0,  0), vec3( 1,  -1,  0), vec3( 1,  -1, -1), vec3(1, -1, 1),
	vec3( 1,  1,  0), vec3( 1,   1, -1), vec3( 1,   1,  1), vec3( 1, 0, -1), vec3( 1, 0, 1)
);

vec3 BlinnPhong(vec3 normalDir, vec3 halfDir, vec3 lightDir, vec3 lightColor, vec3 baseDiffuse, float baseSpecular) {
	vec3 diffuse = lightColor * max(dot(lightDir, normalDir), 0) * INV_PI * baseDiffuse;
	vec3 specular = pow(max(dot(halfDir, normalDir), 0), 64.0) * lightColor * baseSpecular;
	return (diffuse + specular);
}

float FindBlocker(vec3 fragToLight, float currDepth, float bias){
	uint blocker = 0;
	float result = 0.0;
	float r = currDepth * lightNearAndFar.x / lightNearAndFar.y;
	for (int i = 0; i < 27; ++i) {
		float closeDepth = texture(texture_shadow, fragToLight + r * sampleOffsetDirection[i]).x;
		closeDepth *= lightNearAndFar.y;
		if (currDepth - bias > closeDepth){
			result += closeDepth;
			++blocker;
		}
	}
	return blocker == 0 ? -1 : result / blocker;
}

float CalculateShadow(vec3 fragPos, float bias) {
	float shadow = 0.0;
	vec3 fragToLight = fragPos - mainLight.lightPos;
	float currDepth = length(fragToLight);
	float averageDepth = FindBlocker(fragToLight, currDepth, bias);
	if (averageDepth == -1.0)
		return 1.0;
	float penumbra = LIGHT_WIDTH * (currDepth - averageDepth)/ averageDepth; // w = d_width * (drecever - dblocker) / dblocker
	float radius = penumbra * lightNearAndFar.x / currDepth;
	// 它们差不多都是分开的，每一个指向完全不同的方向，剔除彼此接近的那些子方向。
	for (int i = 0; i < 27; ++i) {
		float closeDepth = texture(texture_shadow, fragToLight + radius * sampleOffsetDirection[i]).x;
		closeDepth *= lightNearAndFar.y;
		if (currDepth - bias > closeDepth)
			shadow++;
	}
	shadow /= 27.0;
	return 1.0 - shadow;
}

void main() {
	vec3 fragPos = texture(gPosition, uv).xyz;
	float ao = texture(ambientOcclusion, uv).x;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 normalDir = texture(gNormal, uv).xyz;
	vec3 diffuse = texture(gAlbedo, uv).xyz;
	float specular = texture(gAlbedo, uv).w;

	vec3 lightDir = mainLight.lightPos - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);
	vec3 halfDir = normalize(lightDir + viewDir);
	float bias = max(0.05 * (1.0 - dot(normalDir, lightDir)), 0.005); // 避免表面坡度很大，仍然会产生阴影失真的现象
	float visibility = CalculateShadow(fragPos, bias);
	float atten = 1.0 / (1.0 + 0.7 * dist + 1.8 * dist * dist);
	vec3 ambient = 0.3 * ao * diffuse;
	vec3 result = ambient + visibility * BlinnPhong(normalDir, halfDir, lightDir, mainLight.lightColor, diffuse, specular) * atten;
	result += texture(pointLight, uv).xyz;
	vec3 reflection = texture(screenSpaceReflection, uv).xyz;
	result = mix(result, reflection, 0.5);
	fragColor = vec4(result, 1.0);
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.9){
		brightColor = vec4(result, 1.0);
	} else {
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}