#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 aTangent;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

out VS_OUT{
	vec2 uv;
	vec4 fragPos;
	mat3 tangentSpace;
} vs_out;

uniform int inverseNormal;
uniform mat4 model;

void main() {
	vs_out.fragPos = model * vec4(aPos, 1.0);
	gl_Position = projection * view * vs_out.fragPos;
	vs_out.uv = aTexcoord;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(inverseNormal * normalMatrix * aNormal);
	vec3 tangent = normalize(normalMatrix * aTangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(tangent, normal);
	vs_out.tangentSpace = mat3(tangent, bitangent, normal);
}