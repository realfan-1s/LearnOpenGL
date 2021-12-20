#version 420 core

in vec2 uv;

uniform sampler2D downSampler;
uniform bool horizontal;
uniform float weights[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,0.0540540541, 0.0162162162);

void main() {
	vec2 textelSize = 1.0 / textureSize(downSampler, 0);
	vec3 result = texture(downSampler, uv).xyz * weights[0];
	if (horizontal){
		for (uint i = 1; i < 5; ++i){
			result += texture(downSampler, uv + vec2(textelSize.x * i, 0.0)).xyz * weights[i];
			result += texture(downSampler, uv - vec2(textelSize.x * i, 0.0)).xyz * weights[i];
		}
	} else {
		for (uint i = 1; i < 5; ++i){
			result += texture(downSampler, uv + vec2(0.0, textelSize.y * i)).xyz * weights[i];
			result += texture(downSampler, uv - vec2(0.0, textelSize.y * i)).xyz * weights[i];
		}
	}
	gl_FragColor = vec4(result, 1.0);
}