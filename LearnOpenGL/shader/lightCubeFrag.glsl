#version 420 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
	// ���������������ֵ
	float linearDepth = length(fragPos.xyz - lightPos) / farPlane;
	gl_FragDepth = linearDepth;
}