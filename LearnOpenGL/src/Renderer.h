#pragma once
#include <glad/glad.h>
#include "Shader.h"

/*
 * �ӳ���ɫ�����Ż�������Դ�ĳ������ڵ�һ�����δ���׶Σ�����Ⱦ����һ�λ�ü���˳����G-buffer�д��λ����������ɫ�������������;�������
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
