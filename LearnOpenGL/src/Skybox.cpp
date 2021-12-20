#include "Skybox.h"
#include <string>
#include <iostream>

#include "stb_image.h"

void Skybox::LoadCubeMap(const char* path)
{
	const char** faces = new const char* [6]
	{
		"right.jpg", "left.jpg",
		"top.jpg"  , "bottom.jpg",
		"front.jpg"   , "back.jpg"
	};
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, colorChannel;
	for (int i = 0; i < 6; ++i)
	{
		std::string fileName = string(path) + string(faces[i]);
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &colorChannel, 0);
		if (data)
		{
			GLenum format;
			if (colorChannel == 1)
			    format = GL_RED;
			else if (colorChannel == 3)
			    format = GL_RGB;
			else if (colorChannel == 4)
			    format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else
		{
			std:cout << "CubeMap failed to load" << faces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	delete[] faces;
}

void Skybox::InitCubeMemory()
{
	float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<const void*>(0));
}

Skybox::Skybox(const char* path) {
	this->skyboxShader = new Shader("shader/skyboxVertex.glsl", "shader/skyboxFrag.glsl");
	InitCubeMemory();
	LoadCubeMap(path);
}

void Skybox::Use() const {
	glDepthFunc(GL_LEQUAL);
	skyboxShader->Use();
	skyboxShader->SetInt("skybox", 0);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

unsigned int Skybox::GetSkyboxTexture() const {
	return textureID;
}

Skybox::~Skybox() {
	glDeleteTextures(1, &textureID);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	delete skyboxShader;
}
