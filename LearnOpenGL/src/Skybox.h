#pragma once
#include <glad/glad.h>
#include "Shader.h"

class Skybox {
private:
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int textureID;
	Shader* skyboxShader;
	void LoadCubeMap(const char* path);
	void InitCubeMemory();
public:
	Skybox(const char* path);
	void Use() const;
	unsigned int GetSkyboxTexture() const;
	~Skybox();
};

