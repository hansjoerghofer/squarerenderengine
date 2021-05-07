#pragma once

#include "Camera.h"

DECLARE_PTRS(PerspectiveCamera);

class PerspectiveCamera : public Camera
{
public:

	PerspectiveCamera(int width, int height, float fov, float near, float far);

	virtual ~PerspectiveCamera();

	virtual float fov() const;

	virtual void updateResolution(int width, int height) override;

protected:

	void recomputeProjection();

	float m_fov;
};

