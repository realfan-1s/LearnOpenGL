#include "SSAO.h"

#include <iostream>

#include "DataDefine.h"
#include "Model.h"

SSAO::SSAO(float radius = 0.5f, float bias = 0.025f) : radius(radius), bias(bias) {
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glGenTextures(1, &ssaoTex);
	glBindTexture(GL_TEXTURE_2D, ssaoTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoBlurTex);
	glBindTexture(GL_TEXTURE_2D, ssaoBlurTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ssaoKernel = new glm::vec3[64];
	std::uniform_real_distribution<float> randomFloat(0.0, 1.0);
	std::default_random_engine generator;
	for (int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloat(generator) * 2.0 - 1.0, randomFloat(generator) * 2.0 - 1.0, randomFloat(generator));
		sample = glm::normalize(sample);
		sample *= randomFloat(generator);
		float scale = static_cast<float>(i) / 64.0f;
		scale = LERP(0.1f, 1.0f, scale * scale); // 采样的值越靠近原点越好,因为我们更关注靠近真正片段的遮蔽
		sample *= scale;
		ssaoKernel[i] = sample;
	}
	ssaoNoise = new glm::vec3[16];
	for (int i = 0; i < 16; ++i)
	{
		glm::vec3 noise = glm::vec3(randomFloat(generator) * 2.0 - 1.0, randomFloat(generator) * 2.0 - 1.0, 0.0);
		ssaoNoise[i] = noise; // 引入随机采样减少样本数量因此创建一个旋转向量纹理平铺在屏幕
	}
	glGenTextures(1, &ssaoNoiseTex);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE0, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	ssaoShader = new Shader("shader/ssaoVertex.glsl", "shader/ssaoFrag.glsl");
	ssaoBlurShader = new Shader("shader/ssaoBlurVertex.glsl", "shader/ssaoBlurFrag.glsl");
}

unsigned int SSAO::GetOcclusion() const {
	return ssaoBlurTex;
}

void SSAO::Use(unsigned int gPos, unsigned int gNormal) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	ssaoShader->Use();
	ssaoShader->SetFloat("radius", radius);
	ssaoShader->SetFloat("bias", bias);
	ssaoShader->SetInt("gPos", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPos);
	ssaoShader->SetInt("gNormal", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	ssaoShader->SetInt("ssaoNoise", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);
	for (int i = 0; i < 64; ++i)
	{
		string name = "samples[" + to_string(i) + "]";
		ssaoShader->SetVec3(name.c_str(), ssaoKernel[i]);
	}
	Model::RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	ssaoBlurShader->Use();
	ssaoBlurShader->SetInt("ssaoRaw", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoTex);
	Model::RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

SSAO::~SSAO() {
	delete ssaoKernel;
	delete ssaoNoise;
	delete ssaoBlurShader;
	delete ssaoShader;
	glDeleteFramebuffers(1, &ssaoBlurFBO);
	glDeleteFramebuffers(1, &ssaoFBO);
	glDeleteTextures(1, &ssaoBlurTex);
	glDeleteTextures(1, &ssaoTex);
	glDeleteTextures(1, &ssaoNoiseTex);
}
