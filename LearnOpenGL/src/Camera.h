#pragma once
#include <glm/glm.hpp>

using namespace glm;

class Camera {
private:
	const vec3 worldUp;
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	vec3 cameraRight;
	float yaw;
	float pitch;
	void Update();
	void MouseMovement();
	mat4 view;
	mat4 projection;
	float fov;
	float aspect;
	vec2 nearAndFar;
	unsigned int ubo;
	bool generateUBO;
public:
	mat4 GetViewMat() const;
	mat4 GetProjectionMat() const;
	vec3 GetCameraPos() const;
	vec3 GetForward() const;
	vec2 GetNearAndFar() const;
	void ScrollCallback(float zoomIn);
	Camera(float aspect, float yaw = -90.0f, float pitch = 0.0f, float fov = 60.0f);
	void Straight(float distance);
	void Back(float distance);
	void Left(float distance);
	void Right(float distance);
	void Pitch(float radians);
	void Yaw(float radians);
	void Use();
	~Camera();
};

