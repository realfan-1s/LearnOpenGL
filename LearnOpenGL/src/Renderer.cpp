#include "Renderer.h"
#include "DataDefine.h"

Renderer::Renderer() {
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	attachments[2] = GL_COLOR_ATTACHMENT2;

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// TODO:基于深度值重建世界坐标
	glGenTextures(1, &posBuffer);
	glBindTexture(GL_TEXTURE_2D, posBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posBuffer, 0);
	glGenTextures(1, &normalBuffer);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorBuffer, 0);
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &gBufferRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBufferRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	defferedShader = new Shader("shader/gBufferVertex.glsl", "shader/gBufferFrag.glsl");
}

void Renderer::ComputeDepth(GLint targetFrameBuffer) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer); // 复制GBuffer中的深度到默认帧缓冲
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFrameBuffer);
	glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Renderer::Use(const float cameraNear, const float cameraFar) const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defferedShader->Use();
	defferedShader->SetVec2("nearAndFar", cameraNear, cameraFar);
}

void Renderer::Use(const glm::vec2& nearAndFar) const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defferedShader->Use();
	defferedShader->SetVec2("nearAndFar", nearAndFar);
}


unsigned int Renderer::GetPosBuffer() const {
	return posBuffer;
}

unsigned int Renderer::GetNormalBuffer() const {
	return normalBuffer;
}

unsigned int Renderer::GetColorBuffer() const {
	return colorBuffer;
}

Shader& Renderer::GetGBuffer() const
{
	return *defferedShader;
}

Renderer::~Renderer() {
	delete defferedShader;
	glDeleteRenderbuffers(1, &gBufferRBO);
	glDeleteTextures(1, &colorBuffer);
	glDeleteTextures(1, &normalBuffer);
	glDeleteTextures(1, &posBuffer);
	glDeleteFramebuffers(1, &gBuffer);
}
