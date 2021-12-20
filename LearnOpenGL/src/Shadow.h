#pragma once

#include <glad/glad.h>
#include "Shader.h"

class Shadow {
private:
	unsigned int shadowFBO;
	unsigned int shadowMap;
public:
	Shadow();
	unsigned int GetFBO() const;
	unsigned int GetShadowMap() const;
	void Use() const;
	~Shadow();
};

