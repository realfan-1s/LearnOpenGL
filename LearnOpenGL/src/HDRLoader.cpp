#include "HDRLoader.h"
#include "DataDefine.h"
#include "Model.h"

HDRLoader::HDRLoader(float exposure) : exposure(exposure)
{
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;

	glGenFramebuffers(1, &float32FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, float32FBO);
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	glGenRenderbuffers(1, &float32RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, float32RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, float32RBO);
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 为图像模糊创建两个基本的帧缓冲，每个只有一个颜色缓冲纹理
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffers);
	for (int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
	}

	blur = new Shader("shader/blurVertex.glsl", "shader/blurFrag.glsl");
	hdrShader = new Shader("shader/HDRVertex.glsl", "shader/HDRFrag.glsl");
};

void HDRLoader::BindToFloat32FBO() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, float32FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HDRLoader::Use() const
{
	bool horizontal = false, firstIteration = true;
	// 高斯模糊
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	blur->Use();
	blur->SetInt("downSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	for (unsigned int i = 0; i < 10; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);
		blur->SetBool("horizontal", horizontal);
		Model::RenderQuad();
		horizontal = !horizontal;
		if (firstIteration)
			firstIteration = false;
	}
	// HDR生成
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hdrShader->Use();
	hdrShader->SetFloat("exposure", exposure);
	hdrShader->SetInt("HDRBuffer", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	hdrShader->SetInt("bloomBlur", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
	Model::RenderQuad();
}

unsigned HDRLoader::GetFBO() const
{
	return float32FBO;
}

HDRLoader::~HDRLoader()
{
	delete hdrShader;
	delete blur;
	glDeleteRenderbuffers(1, &float32RBO);
	glDeleteTextures(2, colorBuffers);
	glDeleteFramebuffers(1, &float32FBO);
}
