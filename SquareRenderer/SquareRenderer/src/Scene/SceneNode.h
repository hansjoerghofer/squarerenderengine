#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include "Renderer/IDrawable.h"
#include "Scene/BoundingBox.h"

#include <string>
#include <vector>

DECLARE_PTRS(SceneNode);
DECLARE_PTRS(IGeometry);

class SceneNode : public IDrawable
{
public:

	explicit SceneNode(const std::string& name);

	const std::string& name() const;

	void setParent(SceneNodeSPtr node);

	SceneNodeWPtr parent() const;

	void addChild(SceneNodeSPtr node);

	const std::vector<SceneNodeSPtr>& children() const;

	unsigned int count() const;

	void setLocalTransform(const glm::mat4& transform);

	const glm::mat4& localTransform() const;

	glm::mat4 worldTransform() const;

	void setMaterial(MaterialSPtr material);

	virtual MaterialSPtr material() const override;

	void setGeometry(IGeometrySPtr geometry);

	virtual IGeometrySPtr geometry() const override;

	virtual void preRender(MaterialSPtr material) override;

	virtual void postRender() override;

	void setBounds(const BoundingBox& bounds);

	const BoundingBox& bounds() const;

	BoundingBox hierarchicalBounds() const;

private:
	std::string m_name;

	glm::mat4 m_transform;

	MaterialSPtr m_material;

	IGeometrySPtr m_geometry;

	SceneNodeWPtr m_parent;

	BoundingBox m_bounds;

	std::vector<SceneNodeSPtr> m_children;
};
