#version 450 core
#define INV_PI 0.318309887
#define KERNEL_SIZE 64

out float fragColor;

in vec2 uv;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D ssaoNoise;
uniform vec3 samples[KERNEL_SIZE];
uniform float radius;
uniform float bias;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

const vec2 aspect = vec2(1920.0 / 4.0, 1080.0 / 4.0); // �����������ųߴ�

void main() {
	vec4 view_fragPos = view * vec4(texture(gPos, uv).xyz, 1.0);
	vec3 fragPos = view_fragPos.xyz;
	vec4 view_normal = view * vec4(texture(gNormal, uv).xyz, 0.0);
	vec3 normal = view_normal.xyz;
	// ��Ҫ������ռ�ķ��߱任���۲�ռ�
	vec3 random = normalize(texture(ssaoNoise, uv * aspect).xyz);
	vec3 tangent = normalize(random - dot(random, normal) * normal);
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	float occulsion = 0.0;
	for (uint i = 0; i < KERNEL_SIZE; ++i){
		vec3 sampling = TBN * samples[i]; // �����߿ռ�任���۲�ռ䲢���ϵ�ǰ���ص�λ�ã�
		sampling = fragPos + sampling * radius;
		vec4 offset = vec4(sampling, 1.0);
		offset = projection * offset; // �任���ü��ռ䡢����͸�ӻ��ֲ����ŵ�[0,1]��Χ
		offset.xyz /= offset.w;
		offset.xyz = 0.5 * offset.xyz + 0.5;
		// ������������Ĳ��ֶ��ڱ����Ӳ���Ӱ��
		vec4 offsetPos = view * vec4(texture(gPos, offset.xy).xyz, 1.0);
		float sampleDepth = offsetPos.z;
		float checkEdge = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth)); // ��ǰ���ص���ȺͲ���λ�����֮��
		occulsion += (sampleDepth > sampling.z + bias ? 1.0 : 0.0) * checkEdge;
	}
	occulsion = 1.0 - (occulsion / KERNEL_SIZE); // ���ݺ��ĵĴ�С��׼���ڱι���
	fragColor = occulsion;	
}