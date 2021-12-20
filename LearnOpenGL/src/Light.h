#pragma once

#include "Shader.h"
#include "glm/glm.hpp"

class Light {
private:
	Shader* lightShader;
public:
	Light(const char* vertexName, const char* fragName);
	~Light();
	void Use() const;
	void SetVec3(const char* name, float x, float y, float z) const;
	void SetVec3(const char* name, const glm::vec3& value) const;
	void SetVec4(const char* name, const glm::vec4& value) const;
	void SetVec4(const char* name, float x, float y, float z, float w) const;
	void SetMat4(const char* name, const glm::mat4& mat) const;
	void SetFloat(const char* name, float value) const;
	void SetInt(const char* name, int value) const;
	static float CalculateLightRadius(const glm::vec3& lightColor);
};

