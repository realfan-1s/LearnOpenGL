#version 450 core
#define INV_PI 0.318309887
#define KERNEL_SIZE 64

out float fragColor;

in vec2 uv;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D ssaoNoise;
uniform vec3 samples[64];
uniform float radius;
uniform float bias;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

const vec2 aspect = vec2(1920.0 / 4.0, 1080.0 / 4.0); // 计算噪声缩放尺寸

void main() {
	vec4 view_fragPos = view * vec4(texture(gPos, uv).xyz, 1.0);
	vec3 fragPos = view_fragPos.xyz;
	vec4 view_normal = view * vec4(texture(gNormal, uv).xyz, 0.0);
	vec3 normal = view_normal.xyz;
	// 需要将世界空间的法线变换到观察空间
	vec3 random = normalize(texture(ssaoNoise, uv * aspect).xyz);
	vec3 tangent = normalize(random - dot(random, normal) * normal);
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	float occulsion = 0.0;
	for (uint i = 0; i < KERNEL_SIZE; ++i){
		vec3 sampling = TBN * samples[i]; // 从切线空间变换到观察空间并加上当前像素的位置，
		sampling = fragPos + sampling * radius;
		vec4 offset = vec4(sampling, 1.0);
		offset = projection * offset; // 变换到裁剪空间、进行透视划分并缩放到[0,1]范围
		offset.xyz /= offset.w;
		offset.xyz = 0.5 * offset.xyz + 0.5;
		// 避免非物体表面的部分对遮蔽因子产生影响
		vec4 offsetPos = view * vec4(texture(gPos, offset.xy).xyz, 1.0);
		float sampleDepth = offsetPos.z;
		float checkEdge = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth)); // 当前像素的深度和采样位置深度之差
		occulsion += (sampleDepth > sampling.z + bias ? 1.0 : 0.0) * checkEdge;
	}
	occulsion = 1.0 - (occulsion / KERNEL_SIZE); // 根据核心的大小标准化遮蔽贡献
	fragColor = occulsion;	
}