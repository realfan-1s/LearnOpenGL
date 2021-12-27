#pragma once

#include <glad/glad.h>
#include <sstream>
#include <glm/glm.hpp>

using namespace std;

class Shader {
private:
	GLint programID{0};
	GLint GetUniformID(const char* name) const;
public:
	explicit Shader(const GLchar* computeShaderPath);
	explicit Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	explicit Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);
	Shader(Shader const&) = delete;
	Shader& operator=(const Shader&) = delete;
	void Use() const;
	unsigned int GetProgramID() const;
	void SetVec2(const char* name, float x, float y) const;
	void SetVec2(const char* name, const glm::vec2& value) const;
	void SetVec3(const char* name, float x, float y, float z) const;
	void SetVec3(const char* name, const glm::vec3& value) const;
	void SetVec4(const char* name, const glm::vec4& value) const;
	void SetVec4(const char* name, float x, float y, float z, float w) const;
	void SetBool(const char* name, bool value) const;
	void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetMat4(const char* name, const glm::mat4& value) const;
	void SetVec3Mat(const char* name, const glm::vec3* value, int size) const;
	~Shader();
};