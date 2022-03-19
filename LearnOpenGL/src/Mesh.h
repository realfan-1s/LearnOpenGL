#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include <optional>
#include <variant>

using namespace std;
using namespace glm;

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec3 tangent;
	vec4 boneID;
	vec4 weights;
};
struct Texture
{
	unsigned int id;
	string type;
	string path;
};

template <const char* type>
static variant<std::true_type, std::false_type> JudgeName(const string& name)
{
	if (strcmp(name.c_str(), type))
	{
		return std::false_type();
	}
	return std::true_type();
}

class Mesh {
private:
	unsigned int vao, vbo, ebo;
	void InitMesh();
public:
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	Mesh(vector<unsigned int>& indices, vector<Vertex>& vertices, vector<Texture>& textures);
	void Draw(const Shader& shader) const;
};

