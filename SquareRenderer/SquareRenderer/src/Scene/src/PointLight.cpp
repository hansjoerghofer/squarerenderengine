#include "Scene/PointLight.h"

PointLight::PointLight(
	const glm::vec3& position,
	const glm::vec3& color,
	float intensity)
	: m_position(position)
	, m_color(color)
	, m_intensity(intensity)
{
}

const glm::vec3& PointLight::position() const
{
	return m_position;
}

LightsourceType PointLight::type() const
{
	return LightsourceType::Point;
}

const glm::vec3& PointLight::color() const
{
	return m_color;
}

float PointLight::intensity() const
{
	return m_intensity;
}

bool PointLight::isShadowCaster() const
{
	return false;
}
