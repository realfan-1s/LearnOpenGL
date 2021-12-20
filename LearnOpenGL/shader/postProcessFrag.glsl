#version 450 core
#define INV_PI 0.318309887

in vec2 uv;

uniform sampler2D screenTexture;
uniform vec2 texelSize;

vec3 EdgeDectection(sampler2D sample_Tex, vec2 uv, vec2 texelSize){
	vec3 col = vec3(0, 0, 0);
	vec3 sampleTex[9];
	vec2 offset[9] = vec2[](
		vec2(-1, 1),  vec2(0, 1),  vec2(1, 1),
		vec2(-1, 0),  vec2(0, 0),  vec2(1, 0),
		vec2(-1, -1), vec2(0, -1), vec2(1, -1));
	float kernel[9] = float[](
		1,  1, 1,
		1, -8, 1,
		1,  1, 1);
	for (uint i = 0; i < 9 ;++i){
		sampleTex[i] = texture(sample_Tex, uv + texelSize * offset[i]).xyz;
	}
	for (uint i = 0; i < 9; ++i){
		col += sampleTex[i] * kernel[i];
	}
	return col;
}

void main() {
	vec3 col = texture(screenTexture, uv).xyz;
	gl_FragColor = vec4(col, 1);
}