#pragma once

#include "Common/Macros.h"
#include "Geometry.h"

DECLARE_PTRS(Mesh);

class Mesh : public Geometry
{
public:

    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        bool hasUVs = true,
        bool hasNormals = true,
        bool hasTangents = true
    );

    Mesh(std::vector<Vertex>&& vertices,
         std::vector<uint32_t>&& indices,
        bool hasUVs = true,
        bool hasNormals = true,
        bool hasTangents = true
    );

    virtual ~Mesh() override;

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

protected:

    const std::vector<Vertex> m_vertices;

    const std::vector<uint32_t> m_indices;

    const bool m_hasUVs;
    const bool m_hasNormals;
    const bool m_hasTangents;
};
