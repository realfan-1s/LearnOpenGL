#version 420 core
in vec2 uv;
uniform sampler2D depthMap;
uniform vec2 lightNearAndFar;

float LinearizeDepth(float depth){
	float NDC = 2 * depth -  1; // NDC
	return (2 * lightNearAndFar.x * lightNearAndFar.y) / (lightNearAndFar.x + lightNearAndFar.y - NDC * (lightNearAndFar.y - lightNearAndFar.x));
}

void main() {
	// ortho�����Ὣ�������б������й��߶���ƽ�е�,��perspective���������Ҫ�������Ե����ֵת��Ϊ�������ֵ
	float linearDepth = texture(depthMap, uv).x;
	gl_FragColor = vec4(linearDepth, linearDepth, linearDepth, 1.0f);
}