#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoords;

out vec2 uv;

void main() {
	uv = aTexcoords;
	gl_Position = vec4(aPos, 1.0f);
}