#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out; // 我们输入一个三角形，输出总共6个三角形

uniform mat4 lightMatrix[6];
out vec4 fragPos;

void main() {
	for (int face = 0; face < 6; ++face){
		gl_Layer = face;
		for (int i = 0; i < 3; ++i){
			fragPos = gl_in[i].gl_Position;
			gl_Position = lightMatrix[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}