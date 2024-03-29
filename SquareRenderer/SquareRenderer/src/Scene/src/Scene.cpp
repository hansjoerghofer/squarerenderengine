#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/BoundingBox.h"
#include "Scene/ILightsource.h"
#include "Texture/Cubemap.h"

Scene::Scene(SceneNodeSPtr root)
	: m_root(root)
{
}

SceneNodeSPtr Scene::root() const
{
	return m_root;
}

BoundingBox Scene::sceneBounds() const
{
	return m_root->localTransform() * m_root->hierarchicalBounds();
}

const std::vector<ILightsourceSPtr>& Scene::lights() const
{
	return m_lights;
}

void Scene::addLight(ILightsourceSPtr light)
{
	m_lights.push_back(light);
}

void Scene::setSky(CubemapSPtr sky)
{
	m_sky = sky;
}

CubemapSPtr Scene::sky() const
{
	return m_sky;
}

unsigned int Scene::nodeNum() const
{
	return m_root->count();
}

Scene::Traverser Scene::traverser() const
{
	return Traverser(m_root);
}

Scene::Traverser::Traverser(SceneNodeSPtr node)
	: m_node(node)
{
	for (const SceneNodeSPtr& n : m_node->children())
	{
		m_stack.push(n);
	}
}

bool Scene::Traverser::hasNext() const
{
	return !m_stack.empty();
}

SceneNodeSPtr Scene::Traverser::next()
{
	if (m_stack.empty()) return nullptr;

	SceneNodeSPtr node = m_stack.top();
	m_stack.pop();

	const auto& children = node->children();
	for (auto i = children.rbegin(); i != children.rend(); ++i)
	{
		m_stack.push(*i);
	}

	return node;
}
