#pragma once

#include "Common/Macros.h"
#include "IGeometry.h"

DECLARE_PTRS(Mesh);

class Mesh : public IGeometry
{
public:

    Mesh(const std::vector<Vertex>& vertices, 
         const std::vector<uint32_t>& indices);

    Mesh(std::vector<Vertex>&& vertices,
         std::vector<uint32_t>&& indices);

    virtual ~Mesh() override;

    virtual size_t getVertexCount() const override;

    virtual size_t getVertexBufferSize() const override;

    virtual size_t getIndexCount() const override;

    virtual size_t getIndexBufferSize() const override;

    virtual const std::vector<Vertex>& getVertices() const override;

    virtual const std::vector<uint32_t>& getIndices() const override;

    static MeshUPtr create(std::vector<Vertex>&& vertices,
                           std::vector<uint32_t>&& indices);

protected:

    const std::vector<Vertex> m_vertices;

    const std::vector<uint32_t> m_indices;
};
