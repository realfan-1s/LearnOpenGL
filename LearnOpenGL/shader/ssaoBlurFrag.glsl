#version 450 core
#define INV_PI 0.318309887

in vec2 uv;

uniform sampler2D screenTexture;

void main() {
	vec3 col = texture(screenTexture, uv).rgb;
	gl_FragColor = vec4(col, 1);
}