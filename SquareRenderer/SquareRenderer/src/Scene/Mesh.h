#pragma once

#include "Common/Macros.h"
#include "Geometry.h"

DECLARE_PTRS(Mesh);

class Mesh : public Geometry
{
public:

    Mesh(const std::vector<Vertex>& vertices, 
         const std::vector<uint32_t>& indices);

    Mesh(std::vector<Vertex>&& vertices,
         std::vector<uint32_t>&& indices);

    virtual ~Mesh() override;

    virtual void link(GeometryResourceUPtr resource) override;

    virtual bool hasLink() const override;

    virtual bool isStatic() const override;

    virtual size_t vertexCount() const override;

    virtual size_t vertexBufferSize() const override;

    virtual size_t indexCount() const override;

    virtual size_t indexBufferSize() const override;

    virtual const std::vector<Vertex>& vertices() const override;

    virtual const std::vector<uint32_t>& indices() const override;

    static MeshUPtr create(std::vector<Vertex>&& vertices,
                           std::vector<uint32_t>&& indices);

protected:

    GeometryResourceUPtr m_linkedResource;

    const std::vector<Vertex> m_vertices;

    const std::vector<uint32_t> m_indices;
};
