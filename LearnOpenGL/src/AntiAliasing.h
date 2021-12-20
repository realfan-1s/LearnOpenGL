#pragma once
#include <glad/glad.h>
#include "Shader.h"

class AntiAliasing {
private:
	unsigned int albedo_fbo, albedo_rbo;
	unsigned int multiSampleBuffer;
	// TODO:SMAA
	Shader* edgeDetection, * neighborhood, * blend;
public:
	void Use(unsigned int intermediateFBO) const;
	AntiAliasing(int samples);
	~AntiAliasing();
	unsigned int GetFBO() const;
};

