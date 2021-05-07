#pragma once

#include "Common/Macros.h"
#include "Geometry.h"

DECLARE_PTRS(CubeGeometry);

class CubeGeometry : public Geometry
{
public:

    CubeGeometry();

    virtual ~CubeGeometry();

    virtual void link(GeometryResourceUPtr resource) override;

    virtual bool hasLink() const override;

    virtual bool isStatic() const override;

    virtual size_t vertexCount() const override;

    virtual size_t vertexBufferSize() const override;

    virtual size_t indexCount() const override;

    virtual size_t indexBufferSize() const override;

    virtual const std::vector<Vertex>& vertices() const override;

    virtual const std::vector<uint32_t>& indices() const override;

    static CubeGeometryUPtr create();

protected:

    GeometryResourceUPtr m_linkedResource;

    static const std::vector<Vertex> s_vertices;

    static const std::vector<uint32_t> s_indices;
};
