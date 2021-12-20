#pragma once

#include <vector>
#include "Shader.h"

class HDRLoader {
private:
	unsigned int float32FBO, float32RBO;
	unsigned int colorBuffers[2];
	unsigned int attachments[2];
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffers[2];
	Shader* hdrShader;
	Shader* blur;
public:
	float exposure;
	HDRLoader(float exposure);
	void BindToFloat32FBO() const;
	void Use() const;
	unsigned int GetFBO() const;
	~HDRLoader();
};

