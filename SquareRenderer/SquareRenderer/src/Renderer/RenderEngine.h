#pragma once

#include "Camera.h"

#include "Common/Macros.h"
#include "Scene/Scene.h"

DECLARE_PTRS(RenderEngine);

class RenderEngine
{
public:

	explicit RenderEngine(SceneSPtr scene);

	void render(const Camera& camera);
	
protected:

	SceneSPtr m_scene;
};

