#pragma once

#include "Common/Macros.h"

#include <glm/glm.hpp>

#include <string>
#include <list>

DECLARE_PTRS(SceneNode);
DECLARE_PTRS(Material);
DECLARE_PTRS(Geometry);

class SceneNode
{
public:

	explicit SceneNode(const std::string& name);

	const std::string& name() const;

	void setParent(SceneNodeSPtr node);

	SceneNodeWPtr parent() const;

	void addChild(SceneNodeSPtr node);

	const std::list<SceneNodeSPtr>& children() const;

	void setLocalTransform(const glm::mat4& transform);

	const glm::mat4& localTransform() const;

	glm::mat4 worldTransform() const;

	void setMaterial(MaterialSPtr material);

	MaterialSPtr material() const;

	void setGeometry(GeometrySPtr geometry);

	GeometrySPtr geometry() const;

	bool isDrawable() const;

private:
	std::string m_name;

	glm::mat4 m_transform;

	MaterialSPtr m_material;

	GeometrySPtr m_geometry;

	SceneNodeWPtr m_parent;

	std::list<SceneNodeSPtr> m_children;
};

