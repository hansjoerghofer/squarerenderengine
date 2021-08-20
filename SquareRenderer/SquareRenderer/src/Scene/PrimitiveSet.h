#pragma once

#include "Common/Macros.h"
#include "Scene/IGeometry.h"

#include "API/SharedResource.h"

#include <vector>

DECLARE_PTRS(PrimitiveSet);

enum class PrimitiveType
{
	Points, Lines, Triangles
};

class PrimitiveSet : public IGeometry
{
public:

	PrimitiveSet(PrimitiveType type,
		std::vector<Vertex>&& vertices, 
		bool hasUVs = false,
		bool hasNormals = false,
		bool hasTangents = false);

	virtual ~PrimitiveSet();

	virtual void accept(IGeometryVisitor& visitor) override;

	virtual void bind() override;

	virtual void unbind() override;

	virtual void link(IGeometryResourceUPtr resource);

	virtual bool linked() const;

	virtual bool isBound() const;

	virtual bool isStatic() const;

	virtual PrimitiveType type() const;

	virtual size_t vertexCount() const;

	virtual size_t vertexBufferSize() const;

	virtual const std::vector<Vertex>& vertices() const;

	virtual bool hasUVs() const;

	virtual bool hasNormals() const;

	virtual bool hasTangents() const;

protected:

	PrimitiveType m_type;

	std::vector<Vertex> m_vertices;

	IGeometryResourceUPtr m_linkedResource;

	bool m_isBound = false;

	const bool m_hasUVs;
	const bool m_hasNormals;
	const bool m_hasTangents;
};

