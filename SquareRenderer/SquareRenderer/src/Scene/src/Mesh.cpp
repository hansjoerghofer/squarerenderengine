#include "Scene/Mesh.h"
#include "Resources/SharedResource.h"

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

void Mesh::link(GeometryResourceUPtr resource)
{
    m_linkedResource = std::move(resource);
}

bool Mesh::hasLink() const
{
    return m_linkedResource && m_linkedResource->isValid();
}

bool Mesh::isStatic() const
{
    return true;
}

size_t Mesh::vertexCount() const
{
    return m_vertices.size();
}

size_t Mesh::vertexBufferSize() const
{
    return sizeof(Vertex) * m_vertices.size();
}

size_t Mesh::indexCount() const
{
    return m_indices.size();
}

const std::vector<Vertex>& Mesh::vertices() const
{
    return m_vertices;
}

size_t Mesh::indexBufferSize() const
{
    return sizeof(uint32_t) * m_indices.size();
}

const std::vector<uint32_t>& Mesh::indices() const
{
    return m_indices;
}

MeshUPtr Mesh::create(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
{
    return std::make_unique<Mesh>(vertices, indices);
}
