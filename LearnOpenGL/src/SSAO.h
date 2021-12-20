#pragma once

#include "Shader.h"
#include <random>

/*
 * ��ÿһ��Ƭ�ζ����ݰ���״��Χ�����ֵ����һ���ڱ����ӣ����ڼ�����������Ƭ�εĻ��������
 * �������+ģ��ȥ�������Ͳ���
 */ 
class SSAO {
private:
	unsigned int ssaoFBO, ssaoBlurFBO;
	unsigned int ssaoTex, ssaoBlurTex, ssaoNoiseTex;
	Shader* ssaoShader, *ssaoBlurShader;
	glm::vec3* ssaoKernel, *ssaoNoise;
public:
	float radius, bias;
	SSAO(float radius, float bias);
	unsigned int GetOcclusion() const;
	void Use(unsigned int gPos, unsigned int gNormal) const;
	~SSAO();
};

