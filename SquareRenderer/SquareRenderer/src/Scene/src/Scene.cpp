#include "Scene/Scene.h"

void Scene::addSceneElement(SceneElementSPtr element)
{
	m_elements.push_back(element);
}

const std::vector<SceneElementSPtr>& Scene::sceneElements() const
{
	return m_elements;
}
