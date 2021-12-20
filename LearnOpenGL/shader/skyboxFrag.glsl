#version 450 core

in vec3 uv;

uniform samplerCube skybox;

void main() {
	gl_FragColor = texture(skybox, uv);
}