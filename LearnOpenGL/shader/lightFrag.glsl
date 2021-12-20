#version 420 core
#define INV_PI 0.318309887

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
/*
 1. 定向光源:有方向无衰减
 2. 点光源:四面八方且有光照衰减
 3. 聚光源:只朝一个特定方向而不是所有方向照射光线。这样的结果就是只有在聚光方向的特定半径内的物体才会被照亮，其它的物体都会保持黑暗
*/
struct SpotLight {
	vec3 position;
	float cutoff; // 聚光半径的切光角。落在这个角度之外的物体都不会被这个聚光所照亮
	vec3 direction;
	float outCutoff;
	vec3 ambient;
	float constant;
	vec3 diffuse;
	float linear;
	vec3 specular;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

uniform vec3 lightColor;

//vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normalDir, vec3 viewDir) {
//	vec3 lightDir = normalize(-light.direction);
//	vec3 diffuse = max(dot(normalDir, lightDir), 0) * light.diffuse * INV_PI * texture2D(material.diffuse, uv).xyz;
//	vec3 ambient = texture2D(material.diffuse, uv).xyz * light.ambient;
//	vec3 halfDir = normalize(lightDir + viewDir);
////	vec3 specular = texture2D(material.specular, uv).xyz * pow(max(dot(viewDir, halfDir), 0), material.shininess) * light.specular;
//	return ambient + diffuse + specular;
//}
//
//vec3 CalculatePointLight(PointLight light, vec3 normalDir, vec3 viewDir, vec4 fragPos) {
//	vec3 lightDir = light.position - fragPos.xyz;
//	float distance = length(lightDir);
//	float atten = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
//	lightDir = normalize(lightDir);
//	vec3 halfDir = normalize(viewDir + lightDir);
//	vec3 diffuse = max(dot(normalDir, lightDir), 0) * light.diffuse * INV_PI * texture2D(material.diffuse, uv).xyz;
//	vec3 specular = texture2D(material.specular, uv).xyz * pow(max(dot(viewDir, halfDir), 0), material.shininess) * light.specular;
//	vec3 ambient = texture2D(material.diffuse, uv).xyz * light.ambient;
//	return (diffuse + specular + ambient) * atten;
//}
//
//vec3 CalculateSpotLight(SpotLight light, vec3 normalDir, vec3 viewDir, vec4 fragPos) {
//    vec3 ambient = texture2D(material.diffuse, uv).xyz * light.ambient;
//	vec3 lightDir = light.position - fragPos.xyz;
//	float distance = length(lightDir);
//	float atten = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
//	lightDir = normalize(lightDir);
//	float theta = dot(lightDir, normalize(-light.direction));
//	float epsilon = light.cutoff - light.outCutoff;
//	float intensity = clamp((theta - light.outCutoff) / epsilon, 0, 1);
//	vec3 halfDir = normalize(lightDir + normalDir);
//	vec3 diffuse = max(dot(normalDir, lightDir), 0) * light.diffuse * INV_PI * texture2D(material.diffuse, uv).xyz;
//	vec3 specular = pow(max(dot(viewDir, halfDir), 0), material.shininess) * texture2D(material.specular, uv).xyz * light.specular;
//	return (specular + diffuse + ambient) * atten * intensity;
//}
//
void main() {
//	vec3 result = vec3(0, 0, 0);
//	vec3 normalDir = normalize(normal).xyz;
//	vec3 viewDir = normalize(viewPos - fragPos.xyz);
//	result += CalculateSpotLight(light, normalDir, viewDir, fragPos);
//	result += CalculateDirectionalLight(dirLight, normalDir, viewDir);
//
//	for (uint i = 0; i < MAX_LIGHT_COUNT; ++i) {
//		result += CalculatePointLight(pLights[i], normalDir, viewDir, fragPos);
//	}
//	gl_FragColor = vec4(clamp(result, 0, 1), 1.0f);
	fragColor = vec4(lightColor, 1.0);
	float brightness = dot(lightColor, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.45){
		brightColor = vec4(lightColor, 1.0);
	} else {
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}