#pragma once

#include "Shader.h"
#include <random>

/*
 * 对每一个片段都根据半球状范围的深度值计算一个遮蔽因子，用于减少甚至抵消片段的环境光分量
 * 引入随机+模糊去除噪声和波纹
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

