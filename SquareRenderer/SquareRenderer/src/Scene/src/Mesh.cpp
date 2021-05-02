#include "Scene/Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<uint32_t>& indices)
    : m_vertices(vertices)
    , m_indices(indices)
{
}

Mesh::Mesh(std::vector<Vertex>&& vertices,
           std::vector<uint32_t>&& indices)
    : m_vertices(vertices)
    , m_indices(indices)
{
}

Mesh::~Mesh()
{
}

size_t Mesh::getVertexCount() const
{
    return m_vertices.size();
}

size_t Mesh::getVertexBufferSize() const
{
    return sizeof(Vertex) * m_vertices.size();
}

size_t Mesh::getIndexCount() const
{
    return m_indices.size();
}

const std::vector<Vertex>& Mesh::getVertices() const
{
    return m_vertices;
}

size_t Mesh::getIndexBufferSize() const
{
    return sizeof(uint32_t) * m_indices.size();
}

const std::vector<uint32_t>& Mesh::getIndices() const
{
    return m_indices;
}

MeshUPtr Mesh::create(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
{
    return std::make_unique<Mesh>(vertices, indices);
}
