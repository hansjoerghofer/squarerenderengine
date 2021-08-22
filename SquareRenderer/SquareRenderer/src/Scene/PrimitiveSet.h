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
		unsigned char dataFieldFlags = Vertex::DATA_POSITION);

	virtual ~PrimitiveSet();

	virtual void accept(IGeometryVisitor& visitor) override;

	virtual void bind() override;

	virtual void unbind() override;

	virtual unsigned char dataFieldFlags() const override;

	virtual void link(IGeometryResourceUPtr resource);

	virtual bool linked() const;

	virtual bool isBound() const;

	virtual bool isStatic() const;

	virtual PrimitiveType type() const;

	virtual size_t vertexCount() const;

	virtual size_t vertexBufferSize() const;

	virtual const std::vector<Vertex>& vertices() const;

protected:

	PrimitiveType m_type;

	std::vector<Vertex> m_vertices;

	IGeometryResourceUPtr m_linkedResource;

	bool m_isBound = false;

	const unsigned char m_dataFieldFlags;
};

