#version 420 core
in vec2 uv;
uniform sampler2D depthMap;
uniform vec2 lightNearAndFar;

float LinearizeDepth(float depth){
	float NDC = 2 * depth -  1; // NDC
	return (2 * lightNearAndFar.x * lightNearAndFar.y) / (lightNearAndFar.x + lightNearAndFar.y - NDC * (lightNearAndFar.y - lightNearAndFar.x));
}

void main() {
	// ortho并不会将场景进行变形所有光线都是平行的,而perspective则不是因此需要将非线性的深度值转变为线性深度值
	float linearDepth = texture(depthMap, uv).x;
	gl_FragColor = vec4(linearDepth, linearDepth, linearDepth, 1.0f);
}