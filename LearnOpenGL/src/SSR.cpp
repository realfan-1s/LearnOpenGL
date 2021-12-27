#include "SSR.h"
#include "DataDefine.h"
#include "Model.h"
#include <random>

SSR::SSR() {
	glGenFramebuffers(1, &ssrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
	glGenTextures(1, &ssrTex);
	glBindTexture(GL_TEXTURE_2D, ssrTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	rayMarching = new Shader("shader/rayMarchingVertex.glsl", "shader/rayMarchingFrag.glsl");
}

unsigned SSR::GetSSR() const
{
	return ssrTex;
}

void SSR::Use(unsigned int gPos, unsigned int gNormal, unsigned int gAlbedo) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	rayMarching->Use();
	rayMarching->SetInt("gPos", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPos);
	rayMarching->SetInt("gNormal", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	rayMarching->SetInt("gAlbedo", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	Model::RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

SSR::~SSR() {
	delete rayMarching;
	glDeleteTextures(1, &ssrTex);
	glDeleteFramebuffers(1, &ssrFBO);
}
