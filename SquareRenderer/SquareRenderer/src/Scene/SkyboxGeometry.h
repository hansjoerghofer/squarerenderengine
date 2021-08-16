#pragma once

#include "Common/Macros.h"
#include "Geometry.h"

DECLARE_PTRS(SkyboxGeometry);

class SkyboxGeometry : public Geometry
{
public:

    SkyboxGeometry();

    virtual ~SkyboxGeometry();

    virtual bool isStatic() const override;

    virtual size_t vertexCount() const override;

    virtual size_t vertexBufferSize() const override;

    virtual size_t indexCount() const override;

    virtual size_t indexBufferSize() const override;

    virtual const std::vector<Vertex>& vertices() const override;

    virtual const std::vector<uint32_t>& indices() const override;

    virtual bool hasUVs() const override;

    virtual bool hasNormals() const override;

    virtual bool hasTangents() const override;

    static SkyboxGeometryUPtr create();

protected:

    static const std::vector<Vertex> s_vertices;

    static const std::vector<uint32_t> s_indices;
};
