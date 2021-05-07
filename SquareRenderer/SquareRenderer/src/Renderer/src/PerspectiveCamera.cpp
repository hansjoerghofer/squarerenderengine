#include "Renderer/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(
	int width, int height, float fov, float near, float far)
	: Camera(width, height, near, far)
	, m_fov(fov)
{
	m_view = glm::mat4x4(1.f);
	
	recomputeProjection();
}

PerspectiveCamera::~PerspectiveCamera()
{
}

float PerspectiveCamera::fov() const
{
	return m_fov;
}

void PerspectiveCamera::updateResolution(int width, int height)
{
	Camera::updateResolution(width, height);

	recomputeProjection();
}

void PerspectiveCamera::recomputeProjection()
{
	const float aspectRatio =
		static_cast<float>(height()) / static_cast<float>(width());

	m_projection = glm::perspective(
		glm::radians(fov()), aspectRatio, near(), far());
}
