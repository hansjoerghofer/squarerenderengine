#include "Scene/Scene.h"
#include "Scene/SceneNode.h"

Scene::Scene(SceneNodeSPtr root)
	: m_root(root)
{
}

SceneNodeSPtr Scene::root() const
{
	return m_root;
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

	const std::list<SceneNodeSPtr>& children = node->children();
	for (auto i = children.rbegin(); i != children.rend(); ++i)
	{
		m_stack.push(*i);
	}

	return node;
}
