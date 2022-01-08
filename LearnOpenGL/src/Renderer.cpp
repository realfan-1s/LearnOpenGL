#include "Renderer.h"
#include "Model.h"

Renderer::Renderer() {
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	attachments[2] = GL_COLOR_ATTACHMENT2;
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f};

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glGenTextures(1, &posBuffer);
	glBindTexture(GL_TEXTURE_2D, posBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posBuffer, 0);
	glGenTextures(1, &normalBuffer);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorBuffer, 0);
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &gBufferRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBufferRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &stencilPass);
	glBindFramebuffer(GL_FRAMEBUFFER, stencilPass);
	glGenTextures(1, &stencilPointLight);
	glBindTexture(GL_TEXTURE_2D, stencilPointLight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stencilPointLight, 0);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	deferredShader = new Shader("shader/gBufferVertex.glsl", "shader/gBufferFrag.glsl");
	stencilShader = new Shader("shader/deferStencilVertex.glsl", "shader/deferStencilFrag.glsl");
	pointLightShader = new Shader("shader/vertex.glsl", "shader/pointLightFrag.glsl");
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
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	deferredShader->Use();
	deferredShader->SetVec2("nearAndFar", cameraNear, cameraFar);
}

void Renderer::Use(const glm::vec2& nearAndFar) const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	deferredShader->Use();
	deferredShader->SetVec2("nearAndFar", nearAndFar);
}

// 阴影锥操作
void Renderer::StencilPass(BindState bind, const glm::mat4& model) const
{
	if (bind > BindState::unbind)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, stencilPass);
		glDrawBuffer(GL_NONE);
		glClear(GL_STENCIL_BUFFER_BIT);
		/*
		 * 关闭深度写入=>关闭背面剔除=>模板测试总是通过=>
		 * 多边形背面时模板值递增，正面时模板值递减
		 */
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
		stencilShader->Use();
		stencilShader->SetMat4("model", model);
		Model::RenderSphere();
	} else
	{
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::PointLightPass(const PointLight& light, const glm::vec3& viewPos) const
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glStencilFunc(GL_NOTEQUAL, 0, 0xff);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	pointLightShader->Use();
	pointLightShader->SetInt("gPosition", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posBuffer);
	pointLightShader->SetInt("gNormal", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	pointLightShader->SetInt("gAlbedo", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	pointLightShader->SetVec3("viewPos", viewPos);
	pointLightShader->SetVec3("pointLight.lightPos", light.lightPos);
	pointLightShader->SetVec3("pointLight.lightColor", light.lightColor);
	pointLightShader->SetFloat("pointLight.radius", light.radius);
	Model::RenderQuad();
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

void Renderer::ClearPointLightPass() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, stencilPass);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

unsigned Renderer::GetPointLight() const
{
	return stencilPointLight;
}

Shader& Renderer::GetGBuffer() const
{
	return *deferredShader;
}

Renderer::~Renderer() {
	delete pointLightShader;
	delete stencilShader;
	delete deferredShader;
	glDeleteFramebuffers(1, &stencilPass);
	glDeleteTextures(1, &stencilPointLight);
	glDeleteRenderbuffers(1, &gBufferRBO);
	glDeleteTextures(1, &colorBuffer);
	glDeleteTextures(1, &normalBuffer);
	glDeleteTextures(1, &posBuffer);
	glDeleteFramebuffers(1, &gBuffer);
}
