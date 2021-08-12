#pragma once
#include "Scene/ILightsource.h"
#include "Common/Macros.h"

DECLARE_PTRS(DirectionalLight);

class DirectionalLight : public ILightsource
{
public:

	DirectionalLight(
		const glm::vec3& direction,
		const glm::vec3& color = glm::vec3(1,1,1),
		float intensity = 1.f,
		bool isShadowCaster = true);

	const glm::vec3& direction() const;

	virtual LightsourceType type() const override;

	virtual const glm::vec3& color() const override;

	virtual float intensity() const override;

	virtual bool isShadowCaster() const override;

private:

	glm::vec3 m_direction;

	glm::vec3 m_color;

	float m_intensity;

	bool m_isShadowCaster;
};

