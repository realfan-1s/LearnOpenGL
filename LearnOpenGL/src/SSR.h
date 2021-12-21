#pragma once
#include "Shader.h"

class SSR {
private:
	unsigned int ssrFBO;
	unsigned int ssrTex;
	Shader* rayMarching;
public:
	SSR();
	unsigned int GetSSR() const;
	void Use(unsigned int gPos, unsigned int gNormal, unsigned int gAlbedo) const;
	~SSR();
};

