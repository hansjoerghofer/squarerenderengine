#pragma once

#include "Common/Macros.h"
#include "Renderer/IDrawable.h"
#include "Scene/IGeometry.h"
#include "Material/Material.h"

class Primitive : public IDrawable
{
public:
	Primitive(IGeometrySPtr geometry)
		: m_geometry(geometry)
	{}

	Primitive(IGeometrySPtr geometry, MaterialSPtr material)
		: m_geometry(geometry), m_material(material)
	{}

	virtual IGeometrySPtr geometry() const override
	{
		return m_geometry;
	}

	void setMaterial(MaterialSPtr material)
	{
		m_material = material;
	}

	virtual MaterialSPtr material() const override
	{
		return m_material;
	}

	virtual void preRender(MaterialSPtr /*boundMaterial*/)
	{
	}

	virtual void postRender()
	{
	}

private:
	IGeometrySPtr m_geometry;
	MaterialSPtr m_material;
};