#include "AntiAliasing.h"
#include <iostream>
#include "DataDefine.h"

void AntiAliasing::Use(unsigned int intermediateFBO) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, albedo_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

AntiAliasing::AntiAliasing(int samples)
{
	edgeDetection = new Shader("shader/edgeDetectionVertex.glsl", "shader/edgeDetectionFrag.glsl");
	neighborhood = new Shader("shader/neighborhoodVertex.glsl", "shader/neighborhoodFrag.glsl");
	blend = new Shader("shader/blendVertex.glsl", "Shader/blendFrag.glsl");

	glGenBuffers(1, &albedo_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, albedo_fbo);
	glGenTextures(1, &multiSampleBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multiSampleBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, SCR_WIDTH, SCR_WIDTH, GL_TRUE); // 对每个纹素都采用相同的样本位置和子采样点个数
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multiSampleBuffer, 0);
	glGenBuffers(1, &albedo_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, albedo_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, albedo_rbo);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

AntiAliasing::~AntiAliasing() {
	glDeleteBuffers(1, &albedo_rbo);
	glDeleteTextures(1, &multiSampleBuffer);
	glDeleteBuffers(1, &albedo_fbo);
	delete blend;
	delete neighborhood;
	delete edgeDetection;
}

unsigned AntiAliasing::GetFBO() const
{
	return albedo_fbo;
}
