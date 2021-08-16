#include "Scene/SkyboxGeometry.h"
#include "API/SharedResource.h"

const std::vector<Vertex> SkyboxGeometry::s_vertices = {
    { glm::vec3(-1.0f,  1.0f, -1.0f) },
    { glm::vec3(-1.0f, -1.0f, -1.0f) },
    { glm::vec3( 1.0f, -1.0f, -1.0f) },
    { glm::vec3( 1.0f, -1.0f, -1.0f) },
    { glm::vec3( 1.0f,  1.0f, -1.0f) },
    { glm::vec3(-1.0f,  1.0f, -1.0f) },

    { glm::vec3(-1.0f, -1.0f,  1.0f) },
    { glm::vec3(-1.0f, -1.0f, -1.0f) },
    { glm::vec3(-1.0f,  1.0f, -1.0f) },
    { glm::vec3(-1.0f,  1.0f, -1.0f) },
    { glm::vec3(-1.0f,  1.0f,  1.0f) },
    { glm::vec3(-1.0f, -1.0f,  1.0f) },

    { glm::vec3( 1.0f, -1.0f, -1.0f) },
    { glm::vec3( 1.0f, -1.0f,  1.0f) },
    { glm::vec3( 1.0f,  1.0f,  1.0f) },
    { glm::vec3( 1.0f,  1.0f,  1.0f) },
    { glm::vec3( 1.0f,  1.0f, -1.0f) },
    { glm::vec3( 1.0f, -1.0f, -1.0f) },

    { glm::vec3(-1.0f, -1.0f,  1.0f) },
    { glm::vec3(-1.0f,  1.0f,  1.0f) },
    { glm::vec3( 1.0f,  1.0f,  1.0f) },
    { glm::vec3( 1.0f,  1.0f,  1.0f) },
    { glm::vec3( 1.0f, -1.0f,  1.0f) },
    { glm::vec3(-1.0f, -1.0f,  1.0f) },

    { glm::vec3(-1.0f,  1.0f, -1.0f) },
    { glm::vec3( 1.0f,  1.0f, -1.0f) },
    { glm::vec3( 1.0f,  1.0f,  1.0f) },
    { glm::vec3( 1.0f,  1.0f,  1.0f) },
    { glm::vec3(-1.0f,  1.0f,  1.0f) },
    { glm::vec3(-1.0f,  1.0f, -1.0f) },

    { glm::vec3(-1.0f, -1.0f, -1.0f) },
    { glm::vec3(-1.0f, -1.0f,  1.0f) },
    { glm::vec3( 1.0f, -1.0f, -1.0f) },
    { glm::vec3( 1.0f, -1.0f, -1.0f) },
    { glm::vec3(-1.0f, -1.0f,  1.0f) },
    { glm::vec3( 1.0f, -1.0f,  1.0f) }
};

const std::vector<uint32_t> SkyboxGeometry::s_indices = {};

SkyboxGeometry::SkyboxGeometry()
{
}

SkyboxGeometry::~SkyboxGeometry()
{
}

bool SkyboxGeometry::isStatic() const
{
    return true;
}

size_t SkyboxGeometry::vertexCount() const
{
    return s_vertices.size();
}

size_t SkyboxGeometry::vertexBufferSize() const
{
    return sizeof(Vertex) * s_vertices.size();
}

size_t SkyboxGeometry::indexCount() const
{
    return s_indices.size();
}

const std::vector<Vertex>& SkyboxGeometry::vertices() const
{
    return s_vertices;
}

size_t SkyboxGeometry::indexBufferSize() const
{
    return sizeof(uint32_t) * s_indices.size();
}

const std::vector<uint32_t>& SkyboxGeometry::indices() const
{
    return s_indices;
}

bool SkyboxGeometry::hasUVs() const
{
    return true;
}

bool SkyboxGeometry::hasNormals() const
{
    return true;
}

bool SkyboxGeometry::hasTangents() const
{
    return false;
}

SkyboxGeometryUPtr SkyboxGeometry::create()
{
    return std::make_unique<SkyboxGeometry>();
}
