#include "Shadow.h"
#include "DataDefine.h"

Shadow::Shadow() {
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Shadow::GetFBO() const {
	return shadowFBO;
}

unsigned Shadow::GetShadowMap() const
{
	return shadowMap;
}

void Shadow::Use() const {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

Shadow::~Shadow() {
	glDeleteTextures(1, &shadowMap);
	glDeleteFramebuffers(1, &shadowFBO);
}
