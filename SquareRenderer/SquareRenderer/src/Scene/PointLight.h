#pragma once
#include "Scene/ILightsource.h"

DECLARE_PTRS(PointLight);

class PointLight : public ILightsource
{
public:

	PointLight(
		const glm::vec3& position,
		const glm::vec3& color = glm::vec3(1,1,1),
		float intensity = 1.f);

	const glm::vec3& position() const;

	virtual LightsourceType type() const override;

	virtual const glm::vec3& color() const override;

	virtual float intensity() const override;

	virtual bool isShadowCaster() const override;

private:

	glm::vec3 m_position;

	glm::vec3 m_color;

	float m_intensity;
};

