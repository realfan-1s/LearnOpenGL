#include "Mesh.h"

#include <iostream>

void Mesh::InitMesh() // ������ȷ�Ļ�������ͨ����������ָ�붨�嶥����ɫ���Ĳ���
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	// ���붥�㡢���ߡ���������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, uv)));
	// ת�������߿ռ��ԭ����������ռ������գ����淨������Ҫ�任����������ռ��ÿһ�����淨���������ܲ�ͬ�����¾���任���Ľϴ�
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, tangent)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, boneID)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, weights)));
	glBindVertexArray(0);
}

Mesh::Mesh(vector<unsigned> indices, vector<Vertex> vertices, vector<Texture> textures)
{
	this->indices = indices;
	this->vertices = vertices;
	this->textures = textures;
	InitMesh();
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseCount = 1;
	unsigned int specularCount = 1;
	unsigned int normalCount = 1;
	unsigned int heightCount = 1;
	for (int i = 0; i < textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
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
			std::cout << "�޷������������ " << name << endl;
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