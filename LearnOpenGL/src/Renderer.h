#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include "DataDefine.h"

enum class BindState {
	unbind = 0,
	bind = 1,
};

/*
 * �ӳ���ɫ�����Ż�������Դ�ĳ������ڵ�һ�����δ���׶Σ�����Ⱦ����һ�λ�ü���˳����G-buffer�д��λ����������ɫ�������������;�������
 */
class Renderer
{
private:
	unsigned int gBuffer, gBufferRBO;
	unsigned int posBuffer, normalBuffer, colorBuffer;
	unsigned int attachments[3]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	unsigned int stencilPass, stencilPointLight;
	Shader* deferredShader, *stencilShader, *pointLightShader;
public:
	Renderer();
	void ComputeDepth(GLint targetFrameBuffer = 0) const;
	void Use(float cameraNear, float cameraFar) const;
	void Use(const glm::vec2& nearAndFar) const;
	void StencilPass(BindState bind, const glm::mat4& model) const;
	void PointLightPass(const PointLight& __restrict light, const glm::vec3& __restrict viewPos) const;
	void ClearPointLightPass() const;
	unsigned int GetPosBuffer() const;
	unsigned int GetNormalBuffer() const;
	unsigned int GetColorBuffer() const;
	unsigned int GetPointLight() const;
	Shader& GetGBuffer() const;
	~Renderer();
};
