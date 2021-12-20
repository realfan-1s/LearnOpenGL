#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void Camera::Update()
{
	view = lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
}

void Camera::MouseMovement()
{
	vec3 front;
	front.x = cos(radians(pitch)) * cos(radians(yaw));
	front.y = sin(radians(pitch));
	front.z = cos(radians(pitch)) * sin(radians(yaw));
	cameraFront = normalize(front);
	cameraRight = normalize(cross(cameraFront, worldUp));
	cameraUp = normalize(cross(cameraRight, cameraFront));
	Update();
}

mat4 Camera::GetViewMat() const
{
	return view;
}

mat4 Camera::GetProjectionMat() const
{
	return projection;
}

void Camera::ScrollCallback(float zoomIn)
{
	fov -= zoomIn;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 60.0f)
		fov = 60.0f;
	projection = glm::perspective(radians(fov), aspect, nearAndFar.x, nearAndFar.y);
}

Camera::Camera(float aspect, float yaw, float pitch, float fov)
	: worldUp(0.0f, 1.0f, 0.0f), cameraPos(0.0f, 0.0f, 0.0f), cameraFront(0.0f, 0.0f, -1.0f), cameraUp(worldUp), yaw(yaw), view(1.0f), pitch(pitch), fov(fov), aspect(aspect), nearAndFar(0.1f, 200.0f), generateUBO(false)
{
	//// 施密特正交化
	//vec3 up(0.0f, 1.0f, 0.0f);
	//cameraRight = normalize(cross(cameraDir, up));
	//cameraUp = normalize(cross(cameraDir, cameraRight));
	//// 右向量，上向量，方向向量
	//view = glm::lookAt(cameraRight, cameraUp, cameraDir);
	// view = lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
	cameraRight = normalize(cross(cameraFront, cameraUp));
	Update();
	ScrollCallback(0);
}

void Camera::Straight(float distance)
{
	cameraPos += distance * cameraFront;
	Update();
}

void Camera::Back(float distance)
{
	cameraPos -= distance * cameraFront;
	Update();
}

void Camera::Left(float distance)
{
	cameraPos -= cameraRight * distance;
	Update();
}

void Camera::Right(float distance)
{
	cameraPos += cameraRight * distance;
	Update();
}

void Camera::Pitch(float radians)
{
	pitch -= radians;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	MouseMovement();
}

void Camera::Yaw(float radians)
{
	yaw += radians;
	MouseMovement();
}

void Camera::Use()
{
	if (!generateUBO)
	{
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
		generateUBO = true;
	}
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Camera::~Camera()
{
	if (generateUBO)
		glDeleteBuffers(1, &ubo);
}

vec3 Camera::GetCameraPos() const
{
	return cameraPos;
}

vec3 Camera::GetForward() const
{
	return cameraFront;
}

vec2 Camera::GetNearAndFar() const
{
	return nearAndFar;
}
