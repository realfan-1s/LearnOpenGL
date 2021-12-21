#version 450 core

out float fragColor;

in vec2 uv;

uniform sampler2D ssaoRaw;

void main() {
	vec2 texelSize = 1.0 / textureSize(ssaoRaw, 0);
	float result = 0.0;
	for (int x = -2; x < 2; ++x){
		for (int y = -2; y < 2; ++y){
			vec2 offset = vec2(x * texelSize.x, y * texelSize.y);
			result += texture(ssaoRaw, uv + offset).x;
		}
	}
	fragColor = result / 16.0;
}