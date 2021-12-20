#version 420 core
layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

out vec3 uv;

void main() {
	uv = aPos;
	mat4 skyboxView = mat4(mat3(view));
	vec4 pos = projection * skyboxView * vec4(aPos, 1.0f);
	gl_Position = pos.xyww; // ��ƭ��Ȼ���ʹ����Ϊ���ʼ�յ���1.0
}