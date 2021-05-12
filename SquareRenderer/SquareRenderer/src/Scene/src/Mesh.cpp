#include "Scene/Mesh.h"
#include "API/SharedResource.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<uint32_t>& indices,
           bool hasUVs,
           bool hasNormals,
           bool hasTangents)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_hasUVs(hasUVs)
    , m_hasNormals(hasNormals)
    , m_hasTangents(hasTangents)
{
}

Mesh::Mesh(std::vector<Vertex>&& vertices,
           std::vector<uint32_t>&& indices,
           bool hasUVs,
           bool hasNormals,
           bool hasTangents)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_hasUVs(hasUVs)
    , m_hasNormals(hasNormals)
    , m_hasTangents(hasTangents)
{
}

Mesh::~Mesh()
{
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

bool Mesh::hasUVs() const
{
    return m_hasUVs;
}

bool Mesh::hasNormals() const
{
    return m_hasNormals;
}

bool Mesh::hasTangents() const
{
    return m_hasTangents;
}
