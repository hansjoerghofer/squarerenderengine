#pragma once

#include "Common/Macros.h"
#include "Scene/IGeometry.h"
#include "API/SharedResource.h"

#include <vector>

DECLARE_PTRS(Mesh);

class Mesh : public IGeometry
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

    virtual ~Mesh();

    virtual void accept(IGeometryVisitor& visitor) override;

    virtual void bind() override;

    virtual void unbind() override;

    virtual void link(IGeometryResourceUPtr resource);

    virtual bool linked() const;

    virtual bool isBound() const;

    virtual size_t vertexCount() const;

    virtual size_t vertexBufferSize() const;

    virtual size_t indexCount() const;

    virtual size_t indexBufferSize() const;

    virtual const std::vector<Vertex>& vertices() const;

    virtual const std::vector<uint32_t>& indices() const;

    virtual bool hasUVs() const;

    virtual bool hasNormals() const;

    virtual bool hasTangents() const;

protected:

    const std::vector<Vertex> m_vertices;

    const std::vector<uint32_t> m_indices;

    IGeometryResourceUPtr m_linkedResource;

    bool m_isBound = false;

    const bool m_hasUVs;
    const bool m_hasNormals;
    const bool m_hasTangents;
};
