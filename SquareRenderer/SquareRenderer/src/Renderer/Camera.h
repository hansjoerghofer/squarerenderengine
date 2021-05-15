#pragma once

#include "Common/Macros.h"

#include "glm/glm.hpp"

DECLARE_PTRS(Camera);

class Camera
{
public:

	Camera(int width, int height, float near, float far);

	virtual ~Camera() = 0;

	virtual const glm::vec4& viewport() const;

	virtual int width() const;

	virtual int height() const;

	virtual float near() const;

	virtual float far() const;

	virtual const glm::vec3& position() const;

	virtual const glm::vec3& viewDirection() const;

	virtual const glm::mat4& projectionMatrix() const;

	virtual const glm::mat4& viewMatrix() const;

	virtual void updateResolution(int width, int height);

	virtual void lookAt(
		glm::vec3 position,
		glm::vec3 point,
		glm::vec3 up = glm::vec3(0, 1, 0));

protected:

	glm::vec4 m_viewport;

	int m_width;
	int m_height;

	float m_near;
	float m_far;

	glm::vec3 m_position;
	glm::vec3 m_direction;

	glm::mat4 m_projection;
	glm::mat4 m_view;
};