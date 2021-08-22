#include "Scene/Mesh.h"

#include "API/SharedResource.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<uint32_t>& indices,
           unsigned char dataFieldFlags)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_dataFieldFlags(dataFieldFlags)
{
}

Mesh::Mesh(std::vector<Vertex>&& vertices,
           std::vector<uint32_t>&& indices,
           unsigned char dataFieldFlags)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_dataFieldFlags(dataFieldFlags)
{
}

Mesh::~Mesh()
{
}

void Mesh::accept(IGeometryVisitor& visitor)
{
    visitor.visit(*this);
}

void Mesh::bind()
{
    if (!m_isBound && linked())
    {
        m_linkedResource->bind();
        m_isBound = true;
    }
}

void Mesh::unbind()
{
    if (m_isBound && linked())
    {
        m_linkedResource->unbind();
    }
    m_isBound = false;
}

void Mesh::link(IGeometryResourceUPtr resource)
{
    m_linkedResource = std::move(resource);
}

bool Mesh::linked() const
{
    return m_linkedResource
        && m_linkedResource->isValid();
}

bool Mesh::isBound() const
{
    return m_isBound && linked();
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

unsigned char Mesh::dataFieldFlags() const
{
    return m_dataFieldFlags;
}
