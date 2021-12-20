#include "Light.h"

Light::Light(const char* vertexName, const char* fragName)
{
	lightShader = new Shader(vertexName, fragName);
}

Light::~Light()
{
	delete lightShader;
}

void Light::Use() const
{
	lightShader->Use();
}

void Light::SetVec3(const char* name, float x, float y, float z) const
{
	lightShader->SetVec3(name, x, y, z);
}

void Light::SetVec3(const char* name, const glm::vec3& value) const
{
	lightShader->SetVec3(name, value);
}

void Light::SetVec4(const char* name, const glm::vec4& value) const
{
	lightShader->SetVec4(name, value);
}

void Light::SetVec4(const char* name, float x, float y, float z, float w) const
{
	lightShader->SetVec4(name, x, y, z, w);
}

void Light::SetMat4(const char* name, const glm::mat4& value) const
{
	lightShader->SetMat4(name, value);
}

void Light::SetFloat(const char* name, float value) const
{
	lightShader->SetFloat(name, value);
}

void Light::SetInt(const char* name, int value) const
{
	lightShader->SetInt(name, value);
}

float Light::CalculateLightRadius(const glm::vec3& lightColor)
{
	const float maxBrightness = max(lightColor.x, max(lightColor.y, lightColor.z));
	const float radius = (-0.7 + sqrt(0.7 * 0.7 - 4 * 1.8 * (1.0 - maxBrightness * (256.0 / 5.0)))) / (2 * 1.8);
	return radius;
}
