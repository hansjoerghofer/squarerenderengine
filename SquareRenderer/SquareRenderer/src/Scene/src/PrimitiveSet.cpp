#include "Scene/PrimitiveSet.h"
#include "Scene/IGeometryVisitor.h"

PrimitiveSet::PrimitiveSet(
	PrimitiveType type,
	std::vector<Vertex>&& vertices, 
	bool hasUVs, 
	bool hasNormals, 
	bool hasTangents)
	: m_type(type)
	, m_vertices(vertices)
	, m_hasUVs(hasUVs)
	, m_hasNormals(hasNormals)
	, m_hasTangents(hasTangents)
{
}

PrimitiveSet::~PrimitiveSet()
{
}

void PrimitiveSet::accept(IGeometryVisitor& visitor)
{
	visitor.visit(*this);
}

void PrimitiveSet::bind()
{
	if (!m_isBound && linked())
	{
		m_linkedResource->bind();
		m_isBound = true;
	}
}

void PrimitiveSet::unbind()
{
	if (m_isBound && linked())
	{
		m_linkedResource->unbind();
	}
	m_isBound = false;
}

void PrimitiveSet::link(IGeometryResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

bool PrimitiveSet::linked() const
{
	return m_linkedResource
		&& m_linkedResource->isValid();
}

bool PrimitiveSet::isBound() const
{
	return m_isBound && linked();
}

bool PrimitiveSet::isStatic() const
{
	return true;
}

PrimitiveType PrimitiveSet::type() const
{
	return m_type;
}

size_t PrimitiveSet::vertexCount() const
{
	return m_vertices.size();
}

size_t PrimitiveSet::vertexBufferSize() const
{
	return sizeof(Vertex) * m_vertices.size();
}

const std::vector<Vertex>& PrimitiveSet::vertices() const
{
	return m_vertices;
}

bool PrimitiveSet::hasUVs() const
{
	return m_hasUVs;
}

bool PrimitiveSet::hasNormals() const
{
	return m_hasNormals;
}

bool PrimitiveSet::hasTangents() const
{
	return m_hasTangents;
}
