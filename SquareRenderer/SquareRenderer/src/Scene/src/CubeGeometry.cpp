#include "Scene/CubeGeometry.h"
#include "API/SharedResource.h"

const std::vector<Vertex> CubeGeometry::s_vertices = {
    // front
    { glm::vec3(-.5,-.5, .5), glm::vec2(0,1),  glm::vec3(0, 0, 1) },
    { glm::vec3( .5,-.5, .5), glm::vec2(1,1),  glm::vec3(0, 0, 1) },
    { glm::vec3( .5, .5, .5), glm::vec2(1,0),  glm::vec3(0, 0, 1) },
    { glm::vec3(-.5, .5, .5), glm::vec2(0,0),  glm::vec3(0, 0, 1) },
    // back
    { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -glm::vec3(0, 0, 1) },
    { glm::vec3( .5,-.5,-.5), glm::vec2(1,1), -glm::vec3(0, 0, 1) },
    { glm::vec3( .5, .5,-.5), glm::vec2(1,0), -glm::vec3(0, 0, 1) },
    { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), -glm::vec3(0, 0, 1) },
    // right
    { glm::vec3( .5,-.5, .5), glm::vec2(0,1),  glm::vec3(1, 0, 0) },
    { glm::vec3( .5,-.5,-.5), glm::vec2(1,1),  glm::vec3(1, 0, 0) },
    { glm::vec3( .5, .5,-.5), glm::vec2(1,0),  glm::vec3(1, 0, 0) },
    { glm::vec3( .5, .5, .5), glm::vec2(0,0),  glm::vec3(1, 0, 0) },
    // left
    { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -glm::vec3(1, 0, 0) },
    { glm::vec3(-.5,-.5, .5), glm::vec2(1,1), -glm::vec3(1, 0, 0) },
    { glm::vec3(-.5, .5, .5), glm::vec2(1,0), -glm::vec3(1, 0, 0) },
    { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), -glm::vec3(1, 0, 0) },
    // bottom
    { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -glm::vec3(0, 1, 0) },
    { glm::vec3( .5,-.5,-.5), glm::vec2(1,1), -glm::vec3(0, 1, 0) },
    { glm::vec3( .5,-.5, .5), glm::vec2(1,0), -glm::vec3(0, 1, 0) },
    { glm::vec3(-.5,-.5, .5), glm::vec2(0,0), -glm::vec3(0, 1, 0) },
    // top
    { glm::vec3(-.5, .5, .5), glm::vec2(0,1),  glm::vec3(0, 1, 0) },
    { glm::vec3( .5, .5, .5), glm::vec2(1,1),  glm::vec3(0, 1, 0) },
    { glm::vec3( .5, .5,-.5), glm::vec2(1,0),  glm::vec3(0, 1, 0) },
    { glm::vec3(-.5, .5,-.5), glm::vec2(0,0),  glm::vec3(0, 1, 0) }
};

const std::vector<uint32_t> CubeGeometry::s_indices = {
    // front
    0, 1, 2,
    2, 3, 0,
    // glm::vec3(1, 0, 0)
    8, 9, 10,
    10, 11, 8,
    // back
    7, 6, 5,
    5, 4, 7,
    // left
    12, 13, 14,
    14, 15, 12,
    // bottom
    16, 17, 18,
    18, 19, 16,
    // top
    20, 21, 22,
    22, 23, 20
};

CubeGeometry::CubeGeometry()
{
}

CubeGeometry::~CubeGeometry()
{
}

bool CubeGeometry::isStatic() const
{
    return true;
}

size_t CubeGeometry::vertexCount() const
{
    return s_vertices.size();
}

size_t CubeGeometry::vertexBufferSize() const
{
    return sizeof(Vertex) * s_vertices.size();
}

size_t CubeGeometry::indexCount() const
{
    return s_indices.size();
}

const std::vector<Vertex>& CubeGeometry::vertices() const
{
    return s_vertices;
}

size_t CubeGeometry::indexBufferSize() const
{
    return sizeof(uint32_t) * s_indices.size();
}

const std::vector<uint32_t>& CubeGeometry::indices() const
{
    return s_indices;
}

bool CubeGeometry::hasUVs() const
{
    return true;
}

bool CubeGeometry::hasNormals() const
{
    return true;
}

bool CubeGeometry::hasTangents() const
{
    return false;
}

CubeGeometryUPtr CubeGeometry::create()
{
    return std::make_unique<CubeGeometry>();
}
