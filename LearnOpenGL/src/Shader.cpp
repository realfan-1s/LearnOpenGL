
#include "Shader.h"

#include <iostream>
#include <string>
#include <fstream>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	string vertexCode, fragmentCode;
	ifstream vsFile, fsFile;
	vsFile.exceptions(ifstream::failbit | ifstream::badbit);
	fsFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		vsFile.open(vertexPath);
		fsFile.open(fragmentPath);
		// 将文件内容读入数据流中
		stringstream vsStream, fsStream;
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();
		vsFile.close();
		fsFile.close();
		vertexCode = vsStream.str();
		fragmentCode = fsStream.str();
	}
	catch (ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ \n" << e.what() << "\n" << vertexPath << std::endl;
	}
	const char* c_vertexCode = vertexCode.c_str();
	const char* c_fragmentCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	char* infoLog = new char[1024];
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &c_vertexCode, nullptr);
	glCompileShader(vertex);
	int success;
	// 打印编译错误
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
		cout << infoLog << "\n ERROR::SHADER::VERTEX::COMPILATION_FAILED  " << vertexPath << endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &c_fragmentCode, nullptr);
	glCompileShader(fragment);
	// 打印编译错误
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
		cout << infoLog << "\n ERROR::SHADER::FRAGMENT::COMPILATION_FAILED  " << fragmentPath << endl;
	}
	//  shader Program generate
	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, fragment);
	glLinkProgram(programID);

	glDetachShader(programID, GL_VERTEX_SHADER);
	glDeleteShader(vertex);
	glDetachShader(programID, GL_FRAGMENT_SHADER);
	glDeleteShader(fragment);
	delete[] infoLog;
	infoLog = nullptr;
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
	string vertexCode, fragmentCode, geometryCode;
	ifstream vsFile, fsFile, geometryFile;
	vsFile.exceptions(ifstream::failbit | ifstream::badbit);
	fsFile.exceptions(ifstream::failbit | ifstream::badbit);
	geometryFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		vsFile.open(vertexPath);
		fsFile.open(fragmentPath);
		geometryFile.open(geometryPath);
		// 将文件内容读入数据流中
		stringstream vsStream, fsStream, geometryStream;
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();
		geometryStream << geometryFile.rdbuf();
		vsFile.close();
		fsFile.close();
		geometryFile.close();
		vertexCode = vsStream.str();
		fragmentCode = fsStream.str();
		geometryCode = geometryStream.str();
	}
	catch (ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ \n" << e.what() << std::endl;
	}
	const char* c_vertexCode = vertexCode.c_str();
	const char* c_fragmentCode = fragmentCode.c_str();
	const char* c_geometryCode = geometryCode.c_str();

	unsigned int vertex, fragment, geometry;
	char* infoLog = new char[1024];
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &c_vertexCode, nullptr);
	glCompileShader(vertex);
	int success;
	// 打印编译错误
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
		cout << infoLog << "\n ERROR::SHADER::VERTEX::COMPILATION_FAILED  " << vertexPath << endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &c_fragmentCode, nullptr);
	glCompileShader(fragment);
	// 打印编译错误
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 1024, nullptr, infoLog);
		cout << infoLog << "\n ERROR::SHADER::FRAGMENT::COMPILATION_FAILED  " << fragmentPath << endl;
	}

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &c_geometryCode, nullptr);
	glCompileShader(geometry);
	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometry, 1024, nullptr, infoLog);
		cout << infoLog << "\n ERROR::SHADER::GEOMETRY::COMPILATION_FAILED  " << geometryPath << endl;
	}
	//  shader Program generate
	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, geometry);
	glAttachShader(programID, fragment);
	glLinkProgram(programID);

	glDetachShader(programID, GL_VERTEX_SHADER);
	glDeleteShader(vertex);
	glDetachShader(programID, GL_FRAGMENT_SHADER);
	glDeleteShader(fragment);
	glDetachShader(programID, GL_GEOMETRY_SHADER);
	glDeleteShader(geometry);
	delete[] infoLog;
}

void Shader::Use() const
{
	glUseProgram(programID);
}

unsigned Shader::GetProgramID() const
{
	return programID;
}

void Shader::SetVec2(const char* name, float x, float y) const {
	glUniform2f(GetUniformID(name), x, y);
}

void Shader::SetVec2(const char* name, const glm::vec2& value) const {
	SetVec2(name, value.x, value.y);
}

GLint Shader::GetUniformID(const char* name) const
{
	return glGetUniformLocation(programID, name);
}

Shader::Shader(const GLchar* computeShaderPath)
{
	string computeCode;
	ifstream computeFile;
	computeFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		computeFile.open(computeShaderPath);
		// 将文件内容读入数据流中
		stringstream computeStream;
		computeStream << computeFile.rdbuf();
		computeFile.close();
		computeCode = computeStream.str();
	}
	catch (ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ \n" << e.what() << std::endl;
	}
	const char* c_computeCode = computeCode.c_str();

	unsigned int compute;
	char* infoLog = new char[1024];
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &c_computeCode, nullptr);
	glCompileShader(compute);
	int success;
	// 打印编译错误
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(compute, 1024, nullptr, infoLog);
		cout << infoLog << "\n ERROR::SHADER::COMPUTE_SHADER::COMPILATION_FAILED  " << computeShaderPath << endl;
	}
	//  shader Program generate
	programID = glCreateProgram();
	glAttachShader(programID, compute);
	glLinkProgram(programID);

	glDetachShader(programID, GL_COMPUTE_SHADER);
	glDeleteShader(compute);
	delete[] infoLog;
}

void Shader::SetVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(GetUniformID(name), x, y, z);
}

void Shader::SetVec3(const char* name, const glm::vec3& value) const
{
	SetVec3(name, value.x, value.y, value.z);
}

void Shader::SetVec4(const char* name, const glm::vec4& value) const
{
	SetVec4(name, value.x, value.y, value.z, value.w);
}

void Shader::SetVec4(const char* name, float x, float y, float z, float w) const
{
	glUniform4f(GetUniformID(name), x, y, z, w);
}

void Shader::SetBool(const char* name, bool value) const
{
	glUniform1i(GetUniformID(name), value);
}

void Shader::SetInt(const char* name, int value) const
{
	glUniform1i(GetUniformID(name), value);
}

void Shader::SetFloat(const char* name, float value) const
{
	glUniform1f(GetUniformID(name), value);
}

void Shader::SetMat4(const char* name, const glm::mat4& value) const
{
	glUniformMatrix4fv(GetUniformID(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetVec3Mat(const char* name, const glm::vec3* value, int size) const
{
	glUniform3fv(GetUniformID(name), size, &value[0][0]);
}

Shader::~Shader()
{
	glDeleteShader(programID);
}
