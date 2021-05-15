#pragma once

#include "Common/Macros.h"

#include <glm/glm.hpp>

#include <string>

DECLARE_PTRS(Geometry);
DECLARE_PTRS(Material);
DECLARE_PTRS(SceneElement);

class SceneElement
{
public:
	SceneElement(const std::string& name);
	SceneElement(std::string&& name);

	virtual ~SceneElement();

	virtual std::string name() const;

	virtual const glm::mat4& transform() const;

	virtual GeometrySPtr geometry() const;

	virtual MaterialSPtr material() const;

	virtual void setTransform(const glm::mat4& transform);

	virtual void setGeometry(GeometrySPtr geometry);

	virtual void setMaterial(MaterialSPtr material);

protected:

	std::string m_name;

	glm::mat4 m_transform;

	GeometrySPtr m_geometry;

	MaterialSPtr m_material;
};

