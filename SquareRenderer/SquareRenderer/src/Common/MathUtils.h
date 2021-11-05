#pragma once

#include "Common/Math3D.h"

namespace MathUtils
{
	constexpr float epsylon = 1e-6f;

	bool numericClose(float a, float b, float eps = epsylon);

	bool numericClose(const glm::vec2& a, const glm::vec2& b, float eps = epsylon);

	bool numericClose(const glm::vec3& a, const glm::vec3& b, float eps = epsylon);

	bool numericClose(const glm::vec4& a, const glm::vec4& b, float eps = epsylon);

	glm::vec3 lonLat2Dir(glm::vec2 lonLat);

	glm::vec2 dir2LonLat(glm::vec3 direction);

	glm::mat4 createTransform(
		glm::vec3 eulerRotation, 
		glm::vec3 translate = glm::vec3(0), 
		glm::vec3 scale = glm::vec3(1));
};

