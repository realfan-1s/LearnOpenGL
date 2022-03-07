#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <omp_llvm.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "Model.h"
#include "DataDefine.h"
#include "Renderer.h"
#include "Shadow.h"
#include "HDRLoader.h"
#include "SSAO.h"
#include "SSR.h"

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
	// 设定采用的OpenGL版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, &frameBuffer_Size_Callback);
	glfwSetCursorPosCallback(window, &MouseCallback);
	glfwSetScrollCallback(window, ZoomCallback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// 初始化GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		cout << " 初始化GLAD失败 " << endl;
		return -1;
	}

	// F_depth = (1/z - 1/near) / (1/far - 1/near),此时的深度值是非线性的需要将其变换成NDC再变换成线性深度值
	glEnable(GL_DEPTH_TEST);
	/*
	 * GL_ALWAYS 永远通过深度测试
	 * GL_NEVER 永不通过深度测试
	 * GL_LESS 片段中深度值小于缓冲中深度时通过测试
	 * GL_EQUAL 片段中深度值等于缓冲中深度时通过测试
	 * GL_GREATER 片段中深度值大于缓冲中深度时通过测试
	 * GL_NOTEQUAL 片段中深度值不等于缓冲中深度时通过测试
	 */
	glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/* 防止深度冲突的方法
	 * 不要将物体摆放的太近导致三角形重叠，但需要将每个物体都手动进行调整
	 * 将近平面设置的更远，因为越靠近近平面深度值精度越高
	 * 使用更高精度的深度缓冲
	 */

	// 允许渲染片段时将模板缓冲设定为一个特定的值。通过在渲染时修改模板缓冲的内容。在接下来的渲染迭代中，我们可以读取这些值，来决定丢弃还是保留某个片段。
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//// 定义顶点数据之后，送入顶点着色器阶段。并用顶点缓冲对象在显存中存储顶点数据
	//// VAO记录的是vertex attribute的格式和对应的VBO的名字
	//// ebo 用于设置索引
	//unsigned int vbo, vao;
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);
	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	///*
	//* GL_STATIC_DRAW ：数据不会或几乎不会改变。
	//* GL_DYNAMIC_DRAW：数据会被改变很多。
	//* GL_STREAM_DRAW ：数据每次绘制时都会改变。
	//*/
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	///*
	// * 传入着色器的位置属性
	// * 顶点属性大小
	// * 指定参数类型
	// * 数据是否需要被标准化(-1, 1)之间
	// * 步长和偏移
	// */
	//glVertexAttribPointer(0, 3, GL_FLOAT,   GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));

	SSAO ssao(0.5, 0.025);
	SSR ssr;
	HDRLoader hdr(0.6);
	Renderer renderer;
	Shadow shadow;
	Light light("shader/lightVertex.glsl", "shader/lightFrag.glsl");
	Shader shader("shader/vertex.glsl", "shader/frag.glsl");
	Shader lightShader("shader/lightCubeVertex.glsl", "shader/lightCubeFrag.glsl", "shader/lightCubeGeometry.glsl");
	Model backpack("asset/objects/backpack/backpack.obj", true, true);
	Model cyborg("asset/objects/cyborg/cyborg.obj", false, true);
	unsigned int floor = loadTexture("asset/brickwall.jpg", true);
	unsigned int floor_specular = loadTexture("asset/brickwall_specular.jpg", false);
	unsigned int floor_normal = loadTexture("asset/brickwall_normal.jpg", false);
	// lighting info -------------
	glm::vec3 mainLightPosition(-2.0f, 4.0f, -1.0f);
	glm::vec3 mainLightColor(100.0f);

	float deltaTime = 0.0f, lastFrame = 0.0f;
	float nearPlane = 1.0f, farPlane = 75.0f;
	glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

	vec3 lightPositions[MAX_LIGHT_COUNT];
	vec3 lightColors[MAX_LIGHT_COUNT];
	srand(509);
	#pragma omp parallel
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

	// 生成双缓冲避免图像闪烁问题, 游戏主循环
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		mainLightPosition.y = cos(currentTime * 0.5f) * 3.0f;
		mainLightPosition.z = sin(currentTime * 0.5f) * 3.0f;
		std::vector<glm::mat4> lightMatrix;
		// 右左上下近远
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(1,  0,  0), vec3(0, -1, 0)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(-1, 0,  0), vec3(0, -1, 0)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0,  1,  0), vec3(0, 0, 1)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0, -1,  0), vec3(0, 0, -1)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0,  0,  1), vec3(0, -1, 0)));
		lightMatrix.emplace_back(lightProjection * glm::lookAt(mainLightPosition, mainLightPosition + vec3(0,  0, -1), vec3(0, -1, 0)));

		deltaTime = static_cast<float>(currentTime) - lastFrame;
		lastFrame = static_cast<float>(currentTime);
		// 输入检测
		Input_Monitor(window, deltaTime);
		camera.Use();
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadow.Use();
		lightShader.Use();
		{
			int i = 0;
			std::for_each(lightMatrix.begin(), lightMatrix.end(), [&lightShader, &i](const auto& lightMat) {
				string name = "lightMatrix[" + to_string(i) + "]";
				lightShader.SetMat4(name.c_str(), lightMat);
				++i;
			});
		}
		lightShader.SetVec3("lightPos", mainLightPosition);
		lightShader.SetFloat("farPlane", farPlane);
		glDisable(GL_CULL_FACE);
		glm::mat4 model = glm::mat4(1.0f);
		Model::TransformModel(model, vec3(0), vec4(vec3(0), 1), vec3(7.0f));
		lightShader.SetMat4("model", model);
		Model::RenderCube();
		glEnable(GL_CULL_FACE);
		Model::TransformModel(model, glm::vec3(-5.0f, -2.5f, 2.0f), glm::vec4(vec3(0), 1), glm::vec3(1));
		lightShader.SetMat4("model", model);
		backpack.Draw(lightShader);
		vec4 rotate = vec4(60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		Model::TransformModel(model, glm::vec3(4.0f, 2.0f, -2.0f), rotate, glm::vec3(1, 1, 1));
		lightShader.SetMat4("model", model);
		cyborg.Draw(lightShader);


		// 绘制物体,Geometry阶段
		renderer.Use(camera.GetNearAndFar());
		glDisable(GL_CULL_FACE);
		Model::TransformModel(model, vec3(0), vec4(vec3(0), 1), vec3(7.0f));
		renderer.GetGBuffer().SetMat4("model", model);
		renderer.GetGBuffer().SetInt("inverseNormal", -1);
		renderer.GetGBuffer().SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floor);
		renderer.GetGBuffer().SetInt("texture_specular1", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, floor_specular);
		renderer.GetGBuffer().SetInt("texture_normal1", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floor_normal);
		Model::RenderCube();
		glEnable(GL_CULL_FACE);
		renderer.GetGBuffer().SetInt("inverseNormal", 1);
		Model::TransformModel(model, glm::vec3(-5.0f, -2.5f, 2.0f), glm::vec4(vec3(0), 1), glm::vec3(1));
		renderer.GetGBuffer().SetMat4("model", model);
		backpack.Draw(renderer.GetGBuffer());

		Model::TransformModel(model, glm::vec3(4.0f, 2.0f, -2.0f), rotate, glm::vec3(1, 1, 1));
		renderer.GetGBuffer().SetMat4("model", model);
		cyborg.Draw(renderer.GetGBuffer());

		#pragma omp parallel
		for (unsigned int i = 0; i < MAX_LIGHT_COUNT; ++i)
		{
			float radius = Light::CalculateLightRadius(lightColors[i]);
			Model::TransformModel(model, lightPositions[i], glm::vec4(0, 0, 0, 1), glm::vec3(radius));
			renderer.StencilPass(BindState::bind, model);
			renderer.PointLightPass({lightPositions[i], lightColors[i], radius}, camera.GetCameraPos());
		}
		renderer.StencilPass(BindState::unbind, model);

		ssao.Use(renderer.GetPosBuffer(), renderer.GetNormalBuffer());
		ssr.Use(renderer.GetPosBuffer(), renderer.GetNormalBuffer(), renderer.GetColorBuffer());

		hdr.BindToFloat32FBO();
		shader.Use();
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
		shader.SetInt("screenSpaceReflection", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ssr.GetSSR());
		shader.SetInt("pointLight", 6);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, renderer.GetPointLight());
		shader.SetVec3("viewPos", camera.GetCameraPos());
		shader.SetVec2("lightNearAndFar", nearPlane, farPlane);
		shader.SetVec3("mainLight.lightPos", mainLightPosition);
		shader.SetVec3("mainLight.lightColor", mainLightColor);
		float mainRadius = Light::CalculateLightRadius(mainLightColor);
		shader.SetFloat("mainLight.radius", mainRadius);
		Model::RenderQuad();

		renderer.ComputeDepth(hdr.GetFBO());
		light.Use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, mainLightPosition);
		model = glm::scale(model, glm::vec3(0.5f));
		light.SetMat4("model", model);
		light.SetVec3("lightColor", mainLightColor);
		Model::RenderCube();
		#pragma omp simd
		for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
		{
			Model::TransformModel(model, lightPositions[i], glm::vec4(0, 0, 0, 1), glm::vec3(0.15f));
			light.SetMat4("model", model);
			light.SetVec3("lightColor", lightColors[i]);
			Model::RenderCube();
		}

		// 模糊泛光
		hdr.Use();

		renderer.ClearPointLightPass();
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

unsigned int loadTexture(const char* path, bool gammaCorrection)
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
        else
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}