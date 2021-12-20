#include "Model.h"
#include <iostream>
#include "stb_image.h"

void Model::LoadModel(const char* path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace); // 图元三角化和生成切线向量
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	string dir = path;
	directory = dir.substr(0, dir.find_last_of("/"));
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(ProcessMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
		glm::vec3 vertPos;
		glm::vec2 uv;
		vertPos.x = mesh->mVertices[i].x;
		vertPos.y = mesh->mVertices[i].y;
		vertPos.z = mesh->mVertices[i].z;
		if (mesh->HasNormals())
		{
			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			vertex.normal = normal;
		}
		if (mesh->mTextureCoords[0])
		{
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
			if (mesh->HasTangentsAndBitangents())
			{
				glm::vec3 tangent;
				tangent.x = mesh->mTangents[i].x;
				tangent.y = mesh->mTangents[i].y;
				tangent.z = mesh->mTangents[i].z;
				vertex.tangent = tangent;
			}
		} else
			uv = vec2(0, 0);
		vertex.position = vertPos;
		vertex.uv = uv;
        vertices.push_back(vertex);
    }
	// indices Process
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
		{
			indices.emplace_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	// material Process
    if(mesh->mMaterialIndex >= 0)
    {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps, specularMaps, normalMaps, heightMaps;
    	LoadMaterialTextures(diffuseMaps, material, aiTextureType_DIFFUSE, "texture_diffuse", gammaCorrection);
		LoadMaterialTextures(specularMaps, material, aiTextureType_SPECULAR, "texture_specular", gammaCorrection);
		LoadMaterialTextures(normalMaps, material, aiTextureType_HEIGHT, "texture_normal", false);
		LoadMaterialTextures(heightMaps, material, aiTextureType_AMBIENT, "texture_height", false);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
	return Mesh(indices, vertices, textures);
}

void Model::LoadMaterialTextures(vector<Texture>& target, aiMaterial* material, aiTextureType type, string typeName, bool gammaCorrection)
{
	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (auto& j : texturesLoaded)
		{
			if (strcmp(j.path.data(), str.C_Str()) == 0)
			{
				target.emplace_back(j);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = LoadTextureFromFile(str.C_Str(), directory, false, gammaCorrection);
			texture.type = typeName;
			texture.path = str.C_Str();
			target.emplace_back(texture);
			texturesLoaded.emplace_back(texture);
		}
	}
}

unsigned Model::LoadTextureFromFile(const char* path, const string& directory, bool reverse, bool gammaCorrection) const
{
	string file = directory + '/' + string(path);

	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, colorChannel;
	stbi_set_flip_vertically_on_load_thread(reverse);

	unsigned char* data = stbi_load(file.c_str(), &width, &height, &colorChannel, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (colorChannel == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (colorChannel == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else
	{
		cout << "纹理加载失败, 路径为: " << path << endl;
	}
	stbi_image_free(data);
	return textureID;
}

Model::Model(const char* path, bool gammaCorrection = false) : gammaCorrection(gammaCorrection)
{
	LoadModel(path);
}

void Model::Draw(Shader& shader) const
{
	for (auto mesh : meshes)
	{
		mesh.Draw(shader);
	}
}

unsigned int Model::cubeVAO = 0;
unsigned int Model::cubeVBO = 0;
void Model::RenderCube() {
	// initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
		float finalVertices[11 * 6 * 6];
		for (unsigned int i = 0; i < 288; i += 24)
		{
			glm::vec3 pos1 = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			glm::vec3 pos2 = glm::vec3(vertices[i + 8], vertices[i + 9], vertices[i + 10]);
			glm::vec3 pos3 = glm::vec3(vertices[i + 16], vertices[i + 17], vertices[i + 18]);
			glm::vec2 uv1 = glm::vec2(vertices[i + 6], vertices[i + 7]);
			glm::vec2 uv2 = glm::vec2(vertices[i + 14], vertices[i + 15]);
			glm::vec2 uv3 = glm::vec2(vertices[i + 22], vertices[i + 23]);
			vec3 tangent = CalculateTangent(pos1, pos2, pos3, uv1, uv2, uv3);
			unsigned int layer = i / 8;
			for (unsigned int j = 0; j < 3; ++j)
			{
				unsigned count = (layer + j) * 11;
				finalVertices[count] = vertices[j * 8 + i];
				finalVertices[count +  1] = vertices[j * 8 + i + 1];
				finalVertices[count +  2] = vertices[j * 8 + i + 2];
				finalVertices[count +  3] = vertices[j * 8 + i + 3];
				finalVertices[count +  4] = vertices[j * 8 + i + 4];
				finalVertices[count +  5] = vertices[j * 8 + i + 5];
				finalVertices[count +  6] = vertices[j * 8 + i + 6];
				finalVertices[count +  7] = vertices[j * 8 + i + 7];
				finalVertices[count +  8] = tangent.x;
				finalVertices[count +  9] = tangent.y;
				finalVertices[count + 10] = tangent.z;
			}
		}
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(finalVertices), &finalVertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

glm::vec3 Model::CalculateTangent(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2,
	glm::vec2 uv3)
{
	glm::mat2x3 edgeMat = glm::mat2x3(pos2 - pos1, pos3 - pos1);
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;
	glm::mat2 uvInverse = glm::inverse(glm::mat2(deltaUV1, deltaUV2));
	auto ans = edgeMat * uvInverse;
	return ans[0];
}

unsigned int Model::quadVAO = 0;
unsigned int Model::quadVBO = 0;
void Model::RenderQuad()
{
	if (quadVAO == 0)
    {
        constexpr float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
