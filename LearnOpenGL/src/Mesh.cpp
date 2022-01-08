#include "Mesh.h"

#include <iostream>

void Mesh::InitMesh() // 配置正确的缓冲区并通过顶点属性指针定义顶点着色器的布局
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	// 传入顶点、法线、纹理坐标
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, uv)));
	// 转换到切线空间的原因是在世界空间计算光照，表面法向量需要变换到世界坐标空间而每一个表面法向量都可能不同，导致矩阵变换消耗较大
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, tangent)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, boneID)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, weights)));
	glBindVertexArray(0);
}

Mesh::Mesh(vector<unsigned>& indices, vector<Vertex>& vertices, vector<Texture>& textures)
{
	this->indices = std::move(indices);
	this->vertices = std::move(vertices);
	this->textures = std::move(textures);
	InitMesh();
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseCount = 1;
	unsigned int specularCount = 1;
	unsigned int normalCount = 1;
	unsigned int heightCount = 1;
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuseCount++);
		} else if (name == "texture_specular")
		{
			number = std::to_string(specularCount++);
		} else if (name == "texture_normal")
		{
			number = std::to_string(normalCount++);
		} else if (name == "texture_height")
		{
			number = std::to_string(heightCount++);
		} else
		{
			std::cout << "无法输入此类纹理 " << name << endl;
		}
		shader.SetInt((name + number).c_str(), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}
