#version 450 core
#define MAX_STEPS 64
#define STEP_LENGTH 0.3
#define THICKNESS 10.0
#define MAX_BINARY_STEPS 10
#define MAX_LENGTH (STEP_LENGTH * MAX_STEPS)

out vec4 fragColor;

in vec2 uv;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

vec3 RayMarching(vec3 rayPos, vec3 rayDir){
	vec3 hit = rayPos;
	for (int i = 0; i < MAX_STEPS; ++i){
		vec3 rayStart = hit;// hit 是当前光线的开始点
		hit += STEP_LENGTH * rayDir; // hit 是当前光线的结束点
		float rDepth = hit.z;
		vec4 screenCoord = projection * vec4(hit, 1.0);
		screenCoord /= screenCoord.w;
		if (screenCoord.x < -1 || screenCoord.x > 1 || screenCoord.y < -1 || screenCoord.y > 1)
			return vec3(0);
		vec4 worldPos = vec4(texture(gPos, screenCoord.xy * 0.5 + vec2(0.5)).xyz, 1.0);
		vec4 curPos = view * worldPos;
		float depth = curPos.z - rDepth;
		if (depth > 0.0){
			vec2 hitPixel;
			if (depth < THICKNESS){
				hitPixel = screenCoord.xy * 0.5 + vec2(0.5);
				return texture(gAlbedo, hitPixel).xyz;
			}

			// 二分查找追踪精确数据
			vec3 binaryHit = (rayStart + hit) * 0.5;
			for (int j = 0;j < MAX_BINARY_STEPS; ++j) {
				vec4 binary = projection * vec4(binaryHit, 1.0);
				binary.xyz /= binary.w;
				hitPixel = binary.xy * 0.5 + vec2(0.5);
				vec4 binaryPos = view * vec4(texture(gPos, hitPixel).xyz, 1.0);
				float binaryDepth = binaryPos.z - binaryHit.z;
				if (binaryDepth < 0.03 * THICKNESS){
					return texture(gAlbedo, hitPixel).xyz;
				}

				if (binaryDepth > 0.0){
					hit = binaryHit;
				} else {
					rayStart = binaryHit;
				}
				binaryHit = (rayStart + hit) * 0.5;
			}
			return texture(gAlbedo, hitPixel).xyz;
		}
	}	
	return vec3(0);
}

vec3 RayMarching(vec3 rayPos, vec3 rayDir, vec4 texelSize){
	vec3 rayEnd = rayPos + rayDir * MAX_LENGTH;
	vec4 clipStart = projection * vec4(rayPos, 1.0);
	vec4 clipEnd = projection * vec4(rayEnd, 1.0);
	clipStart /= clipStart.w;
	clipEnd /= clipEnd.w;
	vec2 startUV = clipStart.xy;
	vec2 endUV = clipEnd.xy;
	if (abs(dot(endUV - startUV, endUV - startUV)) < 1e-5)
		endUV += texelSize.xy;

	vec2 deltaPixels = (endUV - startUV) * texelSize.zw;
	float stepSize = min(1.0 / abs(deltaPixels.x), 1.0 / abs(deltaPixels.y)) * MAX_STEPS;
	float samplerScale = 2.0 - min(1.0, rayPos.z / 100);
	stepSize *= samplerScale;

	vec2 currScreenPos = startUV;
	float currDepth = rayPos.z;
	vec2 deltaCurrScreenPos = stepSize * (endUV - startUV);
	float deltaDepth = stepSize * (rayEnd.z - rayPos.z);

	float interpolation = stepSize;
	vec2 prevCurrScreenPos = currScreenPos;
	float prevDepth = currDepth + 0.001 * deltaDepth;
	for (uint i = 0; i < MAX_STEPS && interpolation < 1.0; ++i){
		currScreenPos += deltaCurrScreenPos;
		currDepth += deltaDepth;
		interpolation += stepSize;
		vec4 worldPos = vec4(texture(gPos, currScreenPos.xy * 0.5 + vec2(0.5)).xyz, 1.0);
		vec4 curPos = view * worldPos;
		float depth = curPos.z - currDepth;
		if (depth > 0.0){
			if (depth < THICKNESS){
				return texture(gAlbedo, currScreenPos.xy * 0.5 + vec2(0.5)).xyz;
			}
			vec2 binaryStartPos = prevCurrScreenPos;
			float binaryStartDepth = prevDepth;
			vec2 binaryEndPos = currScreenPos;
			float binaryEndDepth = currDepth;
			vec2 binaryHit = (binaryEndPos + binaryStartPos) * 0.5;
			float binaryHitDepth = (binaryEndDepth + binaryStartDepth) * 0.5;

			for (uint j = 0;j < MAX_BINARY_STEPS; ++j){
				vec4 binaryWorldPos = vec4(texture(gPos, binaryHit.xy * 0.5 + vec2(0.5)).xyz, 1.0);
				vec4 binaryCurPos = view * binaryWorldPos;
				float binaryDepth = binaryCurPos.z - binaryHitDepth;
				if (binaryDepth < 0.1 * THICKNESS)
					return texture(gAlbedo, binaryHit).xyz; 
 
				if (binaryDepth > 0.0){
					binaryEndDepth = binaryHitDepth;
					binaryEndPos = binaryHit;
				} else {
					binaryStartDepth = binaryHitDepth;
					binaryStartPos = binaryHit;
				}
				vec2 binaryHit = (binaryEndPos + binaryStartPos) * 0.5;
				float binaryHitDepth = (binaryEndDepth + binaryStartDepth) * 0.5;
			}
			return texture(gAlbedo, binaryHit).xyz;
		}
		prevCurrScreenPos = currScreenPos;
		prevDepth = currDepth;
	}

	return vec3(0);
}

void main() {
	vec4 world_fragPos = vec4(texture(gPos, uv).xyz, 1.0);
	vec3 fragPos = (view * world_fragPos).xyz;
	vec4 world_normal = view * vec4(texture(gNormal, uv).xyz, 0.0);
	vec3 normal = normalize(world_normal.xyz);
	vec3 reflection = normalize(reflect(fragPos, normal));
	vec4 texelSize = vec4(1 / textureSize(gPos, 0), textureSize(gPos, 0));
	vec3 result = RayMarching(fragPos + 0.001 * normal, reflection, texelSize);
	float specular = texture(gAlbedo, uv).w;
	fragColor = vec4(result * specular, 1.0);
}