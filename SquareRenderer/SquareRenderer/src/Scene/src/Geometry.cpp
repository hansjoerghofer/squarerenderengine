#include "Scene/Geometry.h"

void Geometry::link(IGeometryResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

void Geometry::bind()
{
	if (!m_isBound && linked())
	{
		m_linkedResource->bind();
		m_isBound = true;
	}
}

void Geometry::unbind()
{
	if (m_isBound && linked())
	{
		m_linkedResource->unbind();
	}
	m_isBound = false;
}

bool Geometry::linked() const
{
	return m_linkedResource
		&& m_linkedResource->isValid();
}

bool Geometry::isBound() const
{
	return m_isBound && linked();
}
