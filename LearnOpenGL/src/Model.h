#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class Model {
private:
	vector<Texture> texturesLoaded; // 存储所有已经加载的纹理
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;
	void LoadModel(const char* path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterialTextures(vector<Texture>& target, aiMaterial* material, aiTextureType type, string typeName, bool gamma);
	unsigned int LoadTextureFromFile(const char* path, const string& directory, bool reverse, bool gamma) const;
	static unsigned int cubeVAO;
	static unsigned int cubeVBO;
	static unsigned int quadVAO;
	static unsigned int quadVBO;
public:
	Model(const char* path, bool gammaCorrection);
	void Draw(Shader& shader) const;
	static void RenderCube();
	static glm::vec3 CalculateTangent(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3);
	static void RenderQuad();
};

