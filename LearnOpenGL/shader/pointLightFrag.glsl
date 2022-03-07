#version 450 core
#define INV_PI 0.318309887

out vec3 fragColor;

in vec2 uv;

struct Light{
	vec3 lightPos;
	vec3 lightColor;
	float radius;
};

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform Light pointLight;

vec3 BlinnPhong(vec3 normalDir, vec3 halfDir, vec3 lightDir, vec3 lightColor, vec3 baseDiffuse, float baseSpecular) {
	vec3 diffuse = lightColor * max(dot(lightDir, normalDir), 0) * INV_PI * baseDiffuse;
	vec3 specular = pow(max(dot(halfDir, normalDir), 0), 64.0) * lightColor * baseSpecular;
	return (diffuse + specular);
}


void main() {
	vec3 fragPos = texture(gPosition, uv).xyz;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 normalDir = texture(gNormal, uv).xyz;
	vec3 diffuse = texture(gAlbedo, uv).xyz;
	float specular = texture(gAlbedo, uv).w;
	vec3 lightDir = pointLight.lightPos - fragPos;

	vec3 result = vec3(0);
	float dist = length(lightDir);
	if (dist < pointLight.radius) {
		lightDir = normalize(lightDir);
		vec3 halfDir = normalize(lightDir + viewDir);
		float atten = 1.0 / (1.0 + 0.7 * dist + 1.8 * dist * dist);
		result += BlinnPhong(normalDir, halfDir, lightDir, pointLight.lightColor, diffuse, specular) * atten;
	}
	fragColor = result;
}