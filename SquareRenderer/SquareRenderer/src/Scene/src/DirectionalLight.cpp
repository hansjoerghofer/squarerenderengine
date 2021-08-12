#include "Scene/DirectionalLight.h"

DirectionalLight::DirectionalLight(
	const glm::vec3& direction, 
	const glm::vec3& color, 
	float intensity, bool isShadowCaster)
	: m_direction(glm::normalize(direction))
	, m_color(color)
	, m_intensity(intensity)
	, m_isShadowCaster(isShadowCaster)
{
}

const glm::vec3& DirectionalLight::direction() const
{
	return m_direction;
}

LightsourceType DirectionalLight::type() const
{
	return LightsourceType::Directional;
}

const glm::vec3& DirectionalLight::color() const
{
	return m_color;
}

float DirectionalLight::intensity() const
{
	return m_intensity;
}

bool DirectionalLight::isShadowCaster() const
{
	return m_isShadowCaster;
}
