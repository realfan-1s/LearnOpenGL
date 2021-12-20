#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "Model.h"
#include "DataDefine.h"
#include "Renderer.h"
#include "Skybox.h"
#include "Shadow.h"
#include "HDRLoader.h"
#include "SSAO.h"

using namespace std;

void frameBuffer_Size_Callback(GLFWwindow* window, int width, int height);
void Input_Monitor(GLFWwindow* window, float deltaTime);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void ZoomCallback(GLFWwindow* window, double xPos, double yPos);
unsigned int loadTexture(char const* path, bool gammaCorrection);

constexpr float quadVertices[] = {
	// positions        // texture Coords
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

static const float aspect = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
Camera camera(aspect);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int main() {
	glfwInit();
	// �趨���õ�OpenGL�汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// �������ڶ���
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, &frameBuffer_Size_Callback);
	glfwSetCursorPosCallback(window, &MouseCallback);
	glfwSetScrollCallback(window, ZoomCallback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// ��ʼ��GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		cout << " ��ʼ��GLADʧ�� " << endl;
		return -1;
	}

	// F_depth = (1/z - 1/near) / (1/far - 1/near),��ʱ�����ֵ�Ƿ����Ե���Ҫ����任��NDC�ٱ任���������ֵ
	glEnable(GL_DEPTH_TEST);
	/*
	 * GL_ALWAYS ��Զͨ����Ȳ���
	 * GL_NEVER ����ͨ����Ȳ���
	 * GL_LESS Ƭ�������ֵС�ڻ��������ʱͨ������
	 * GL_EQUAL Ƭ�������ֵ���ڻ��������ʱͨ������
	 * GL_GREATER Ƭ�������ֵ���ڻ��������ʱͨ������
	 * GL_NOTEQUAL Ƭ�������ֵ�����ڻ��������ʱͨ������
	 */
	glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/* ��ֹ��ȳ�ͻ�ķ���
	 * ��Ҫ������ڷŵ�̫�������������ص�������Ҫ��ÿ�����嶼�ֶ����е���
	 * ����ƽ�����õĸ�Զ����ΪԽ������ƽ�����ֵ����Խ��
	 * ʹ�ø��߾��ȵ���Ȼ���
	 */

	// ������ȾƬ��ʱ��ģ�建���趨Ϊһ���ض���ֵ��ͨ������Ⱦʱ�޸�ģ�建������ݡ��ڽ���������Ⱦ�����У����ǿ��Զ�ȡ��Щֵ���������������Ǳ���ĳ��Ƭ�Ρ�
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//// ���嶥������֮�����붥����ɫ���׶Ρ����ö��㻺��������Դ��д洢��������
	//// VAO��¼����vertex attribute�ĸ�ʽ�Ͷ�Ӧ��VBO������
	//// ebo ������������
	//unsigned int vbo, vao;
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);
	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	///*
	//* GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
	//* GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
	//* GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
	//*/
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	///*
	// * ������ɫ����λ������
	// * �������Դ�С
	// * ָ����������
	// * �����Ƿ���Ҫ����׼��(-1, 1)֮��
	// * ������ƫ��
	// */
	//glVertexAttribPointer(0, 3, GL_FLOAT,   GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));

	Renderer renderer;
	Shadow shadow;
	HDRLoader hdr(0.6f);
	SSAO ssao(0.5f, 0.025f);
	Light light("shader/lightVertex.glsl", "shader/lightFrag.glsl");
	Shader shader("shader/vertex.glsl", "shader/frag.glsl");
	Shader quadShader("shader/quadVertex.glsl", "shader/quadFrag.glsl");
	Shader lightShader("shader/lightCubeVertex.glsl", "shader/lightCubeFrag.glsl", "shader/lightCubeGeometry.glsl");
	Model nanosuit("asset/objects/nanosuit/nanosuit.obj", true);
	Model backpack("asset/objects/backpack/backpack.obj", true);
	unsigned int floor = loadTexture("asset/brickwall.jpg", true);
	unsigned int floor_normal = loadTexture("asset/brickwall_normal.jpg", false);
	// lighting info -------------
	glm::vec3 mainLightPosition = glm::vec3(-2.0f, 4.0f, -1.0f);
	glm::vec3 mainLightColor = glm::vec3(100.0f);

	float deltaTime = 0.0f, lastFrame = 0.0f;
	float nearPlane = 1.0f, farPlane = 75.0f;
	glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

	vec3 lightPositions[MAX_LIGHT_COUNT];
	vec3 lightColors[MAX_LIGHT_COUNT];
	srand(509);
	for (int i = 0 ;i < MAX_LIGHT_COUNT; ++i)
	{
		float xPos = ((rand() % 100) / 100.0) * 12.0 - 6.0f;
        float yPos = ((rand() % 100) / 100.0) * 12.0 - 6.0f;
        float zPos = ((rand() % 100) / 100.0) * 12.0 - 6.0f;
		lightPositions[i] = vec3(xPos, yPos, zPos);
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors[i] = vec3(rColor, gColor, bColor);
	}

	// ����˫�������ͼ����˸����, ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		mainLightPosition.y = cos(currentTime * 0.5f) * 3.0f;
		mainLightPosition.z = sin(currentTime * 0.5f) * 3.0f;
		std::vector<glm::mat4> lightMatrix;
		// �������½�Զ
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(1,  0,  0), vec3(0, -1, 0)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(-1, 0,  0), vec3(0, -1, 0)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0,  1,  0), vec3(0, 0, 1)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0, -1,  0), vec3(0, 0, -1)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0,  0,  1), vec3(0, -1, 0)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0,  0, -1), vec3(0, -1, 0)));

		deltaTime = static_cast<float>(currentTime) - lastFrame;
		lastFrame = static_cast<float>(currentTime);
		// ������
		Input_Monitor(window, deltaTime);
		camera.Use();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadow.Use();
		lightShader.Use();
		for (unsigned int i = 0; i < 6; ++i)
		{
			string name = "lightMatrix[" + to_string(i) + "]";
			lightShader.SetMat4(name.c_str(), lightMatrix[i]);
		}
		lightShader.SetVec3("lightPos", mainLightPosition);
		lightShader.SetFloat("farPlane", farPlane);
		glDisable(GL_CULL_FACE);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		lightShader.SetMat4("model", model);
		Model::RenderCube();
		glEnable(GL_CULL_FACE);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -2.0f, 2.0f));
		lightShader.SetMat4("model", model);
		backpack.Draw(lightShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 2.0f, -2.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		lightShader.SetMat4("model", model);
		nanosuit.Draw(lightShader);

		// ��������,Geometry�׶�
		renderer.Use(camera.GetNearAndFar());
		glDisable(GL_CULL_FACE);
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		renderer.GetGBuffer().SetMat4("model", model);
		renderer.GetGBuffer().SetInt("inverseNormal", -1);
		renderer.GetGBuffer().SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floor);
		renderer.GetGBuffer().SetInt("texture_normal1", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, floor_normal);
		renderer.GetGBuffer().SetInt("texture_specular1", 2);
		glActiveTexture(GL_TEXTURE2);
		Model::RenderCube();
		glEnable(GL_CULL_FACE);
		renderer.GetGBuffer().SetInt("inverseNormal", 1);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -2.0f, 2.0f));
		renderer.GetGBuffer().SetMat4("model", model);
		backpack.Draw(renderer.GetGBuffer());
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 2.0f, -2.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		renderer.GetGBuffer().SetMat4("model", model);
		nanosuit.Draw(renderer.GetGBuffer());

		ssao.Use(renderer.GetPosBuffer(), renderer.GetNormalBuffer());

		//hdr.BindToFloat32FBO();
		shader.Use();
		shader.SetVec3("viewPos", camera.GetCameraPos());
		shader.SetInt("gPosition", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderer.GetPosBuffer());
		shader.SetInt("gNormal", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderer.GetNormalBuffer());
		shader.SetInt("gAlbedo", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, renderer.GetColorBuffer());
		shader.SetInt("texture_shadow", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow.GetShadowMap());
		shader.SetInt("ambientOcclusion", 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ssao.GetOcclusion());
		shader.SetVec2("lightNearAndFar", nearPlane, farPlane);
		shader.SetVec3("mainLight.lightPos", mainLightPosition);
		shader.SetVec3("mainLight.lightColor", mainLightColor);
		float mainRadius = Light::CalculateLightRadius(mainLightColor);
		shader.SetFloat("mainLight.radius", mainRadius);
		for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
		{
			shader.SetVec3(("lights[" + to_string(i) + "].lightPos").c_str(), lightPositions[i]);
			shader.SetVec3(("lights[" + to_string(i) + "].lightColor").c_str(), lightColors[i]);
			float radius = Light::CalculateLightRadius(lightColors[i]);
			shader.SetFloat(("lights[" + to_string(i) + "].radius").c_str(), radius);
		}
		Model::RenderQuad();

		//renderer.ComputeDepth(hdr.GetFBO());
		//light.Use();
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, mainLightPosition);
		//model = glm::scale(model, glm::vec3(0.5f));
		//light.SetMat4("model", model);
		//light.SetVec3("lightColor", mainLightColor);
		//Model::RenderCube();
		//for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
		//{
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, lightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.15f));
		//	light.SetMat4("model", model);
		//	light.SetVec3("lightColor", lightColors[i]);
		//	Model::RenderCube();
		//}

		//// ģ������
		//hdr.Use();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void frameBuffer_Size_Callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Input_Monitor(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
	float cameraSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.Straight(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.Back(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.Left(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.Right(cameraSpeed);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		firstMouse = false;
		lastX = xPos;
		lastY = yPos;
	}
	float xOffset = (xPos - lastX) * 0.1f;
	float yOffset = (yPos - lastY) * 0.1f;
	lastX = xPos;
	lastY = yPos;

	camera.Yaw(xOffset);
	camera.Pitch(yOffset);
}

void ZoomCallback(GLFWwindow* window, double xPos, double yPos)
{
	camera.ScrollCallback(yPos);
}

unsigned int loadTexture(char const * path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}