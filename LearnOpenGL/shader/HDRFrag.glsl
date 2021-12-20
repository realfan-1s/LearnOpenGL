#version 450 core

in vec2 uv;

uniform sampler2D HDRBuffer;
uniform float exposure;
uniform sampler2D bloomBlur;

vec3 ACESToneMapping(vec3 color){
	const float A = 2.51;
	const float B = 0.03;
	const float C = 2.43;
	const float D = 0.59;
	const float E = 0.14;
	color *= exposure;
	return (color * (A * color + B)) / (color * (C * color + D) +  E);
}

void main() {
	vec3 hdrColor = texture(HDRBuffer, uv).xyz;
	hdrColor += texture(bloomBlur, uv).xyz;
	vec3 result = ACESToneMapping(hdrColor);
	result = pow(result, vec3(1.0 / 2.2));
	gl_FragColor = vec4(result, 1.0);
}