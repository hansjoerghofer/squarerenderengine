#include "Common/MathUtils.h"

bool MathUtils::numericClose(float a, float b, float eps /*= epsylon*/)
{
	return fabs(a - b) < eps;
}

bool MathUtils::numericClose(const glm::vec2& a, const glm::vec2& b, float eps /*= epsylon*/)
{
	return numericClose(a.x, b.x, eps)
		&& numericClose(a.y, b.y, eps);
}

bool MathUtils::numericClose(const glm::vec3& a, const glm::vec3& b, float eps /*= epsylon*/)
{
	return numericClose(a.x, b.x, eps)
		&& numericClose(a.y, b.y, eps)
		&& numericClose(a.z, b.z, eps);
}

bool MathUtils::numericClose(const glm::vec4& a, const glm::vec4& b, float eps /*= epsylon*/)
{
	return numericClose(a.x, b.x, eps)
		&& numericClose(a.y, b.y, eps)
		&& numericClose(a.z, b.z, eps)
		&& numericClose(a.w, b.w, eps);
}

glm::vec3 MathUtils::lonLat2Dir(glm::vec2 lonLat)
{
	const glm::vec2 sphericalCoords = glm::vec2(
		(1.f - lonLat.x) * 2.f * glm::pi<float>(), 
		lonLat.y * glm::pi<float>());

	const glm::vec3 direction = -glm::vec3(
		std::sinf(sphericalCoords.x) * std::sinf(sphericalCoords.y),
		std::cosf(sphericalCoords.y),
		std::cosf(sphericalCoords.x) * std::sinf(sphericalCoords.y)
	);

	return glm::normalize(direction);
}

glm::vec2 MathUtils::dir2LonLat(glm::vec3 direction)
{
	direction = glm::normalize(direction);

	const glm::vec2 sphericalCoords = glm::vec2(
		std::atan2f(direction.z, direction.x),
		std::asinf(direction.y)
	);

	constexpr glm::vec2 iSphere(1 / (2 * glm::pi<float>()), 1 / glm::pi<float>());
	constexpr glm::vec2 offset(0.25, 0.5);

	/*glm::vec2 lonLat = glm::vec2(
		(sphericalCoords.x - (2 * glm::pi<float>())) / std::cosf(sphericalCoords.y),
		sphericalCoords.y - glm::pi<float>()
	);

	// normalize
	lonLat.x /= 2 * glm::pi<float>();
	lonLat.y /= glm::pi<float>();*/

	const glm::vec2 uv = sphericalCoords * iSphere;

	return uv + offset;
}

glm::mat4 MathUtils::createTransform(glm::vec3 eulerRotation, glm::vec3 translate, glm::vec3 scale)
{
	glm::mat4 R = glm::eulerAngleYXZ(
		glm::radians(eulerRotation.y),
		glm::radians(eulerRotation.x),
		glm::radians(eulerRotation.z));
	glm::mat4 S = glm::translate(glm::mat4(1), translate);
	glm::mat4 T = glm::scale(glm::mat4(1), glm::vec3(scale));

	return S * T * R;
}
