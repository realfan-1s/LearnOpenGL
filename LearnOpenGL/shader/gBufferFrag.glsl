#version 450 core
#define INV_PI 0.318309887

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in VS_OUT{
	vec2 uv;
	vec4 fragPos;
	mat3 tangentSpace;
} fs_in;

uniform vec2 nearAndFar;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

float LinearizeDepth(float depth) {
	float NDC = 2 * depth - 1;
	return (2 * nearAndFar.x * nearAndFar.y) / (nearAndFar.x + nearAndFar.y - NDC * (nearAndFar.y - nearAndFar.x));
}

void main() {
	// 有光照计算，保证所有变量在一个坐标空间当中至关重要。
	gPosition = fs_in.fragPos.xyz;
	vec3 normalDir = texture(texture_normal1, fs_in.uv).xyz;
	normalDir = 2 * normalDir - 1;
	gNormal.xyz = normalize(fs_in.tangentSpace * normalDir);
	gNormal.w = LinearizeDepth(gl_FragCoord.z);
	gAlbedo.xyz = texture(texture_diffuse1, fs_in.uv).xyz;
	gAlbedo.w = texture(texture_specular1, fs_in.uv).x;
}