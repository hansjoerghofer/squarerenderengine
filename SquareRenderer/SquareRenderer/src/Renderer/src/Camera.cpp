#include "Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int width, int height, float near, float far)
	: m_width(width)
	, m_height(height)
	, m_near(near)
	, m_far(far)
	, m_view(1.f)
	, m_projection(1.f)
	, m_position(0,0,0)
	, m_direction(0,0,1)
{
}

Camera::~Camera()
{
}

int Camera::width() const
{
	return m_width;
}

int Camera::height() const
{
	return m_height;
}

float Camera::near() const
{
	return m_near;
}

float Camera::far() const
{
	return m_far;
}

const glm::vec3& Camera::position() const
{
	return m_position;
}

const glm::vec3& Camera::viewDirection() const
{
	return m_direction;
}

const glm::mat4& Camera::projectionMatrix() const
{
	return m_projection;
}

const glm::mat4& Camera::viewMatrix() const
{
	return m_view;
}

void Camera::updateResolution(int width, int height)
{
	m_width = width;
	m_height = height;
}

void Camera::lookAt(glm::vec3 position, glm::vec3 point, glm::vec3 up)
{
	m_view = glm::lookAt(position, point, up);

	m_position = position;
	m_direction = glm::normalize(point - position);
}
