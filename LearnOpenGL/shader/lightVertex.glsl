#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 aTangent;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}