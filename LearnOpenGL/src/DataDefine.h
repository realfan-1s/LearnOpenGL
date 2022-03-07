#pragma once
#include <variant>

#define SCR_WIDTH    1920
#define SCR_HEIGHT   1080
#define SHADOW_WIDTH  2048
#define SHADOW_HEIGHT 2048
#define MAX_LIGHT_COUNT 40
#define LIGHT_WIDTH 0.5
#define LERP(a, b, f) a + f * (b - a)
#define PI 3.141592654
#define INV_PI 0.318309887

struct PointLight
{
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	float radius;
};

inline auto MakeBoolVariant(bool x)->std::variant<std::false_type, std::true_type> // �������ͬ���뵥Ԫ���ã�ÿ�����뵥Ԫ���ã����ӵ���ͬһ������ͬһ���塣
{
 	if (x)
		return std::true_type();
 	return std::false_type();
 } 