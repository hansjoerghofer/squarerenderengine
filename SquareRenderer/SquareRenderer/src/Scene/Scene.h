#pragma once

#include "Common/Macros.h"

#include <vector>

DECLARE_PTRS(SceneElement);
DECLARE_PTRS(Scene);

class Scene
{
public:

	void addSceneElement(SceneElementSPtr element);

	const std::vector<SceneElementSPtr>& sceneElements() const;
	
protected:

	std::vector<SceneElementSPtr> m_elements;
};

