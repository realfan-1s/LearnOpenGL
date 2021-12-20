#pragma once
#include <glad/glad.h>
#include "Shader.h"

/*
 * 延迟着色可以优化大量光源的场景，在第一个几何处理阶段，先渲染场景一次获得几何顺序，在G-buffer中存放位置向量、颜色向量、法向量和镜面向量
 */
class Renderer
{
private:
	unsigned int gBuffer, gBufferRBO;
	unsigned int posBuffer, normalBuffer, colorBuffer;
	unsigned int attachments[3];
	Shader* defferedShader;
public:
	Renderer();
	void ComputeDepth(GLint targetFrameBuffer) const;
	void Use(const float cameraNear, const float cameraFar) const;
	void Use(const glm::vec2& nearAndFar) const;
	unsigned int GetPosBuffer() const;
	unsigned int GetNormalBuffer() const;
	unsigned int GetColorBuffer() const;
	Shader& GetGBuffer() const;
	~Renderer();
};
