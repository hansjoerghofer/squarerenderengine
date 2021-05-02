#include "Scene/CubeGeometry.h"

const std::vector<Vertex> CubeGeometry::vertices = {
    // front
    { glm::vec3(-.5,-.5, .5), glm::vec2(0,1), FORWARD },
    { glm::vec3( .5,-.5, .5), glm::vec2(1,1), FORWARD },
    { glm::vec3( .5, .5, .5), glm::vec2(1,0), FORWARD },
    { glm::vec3(-.5, .5, .5), glm::vec2(0,0), FORWARD },
    // back
    { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -FORWARD },
    { glm::vec3( .5,-.5,-.5), glm::vec2(1,1), -FORWARD },
    { glm::vec3( .5, .5,-.5), glm::vec2(1,0), -FORWARD },
    { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), -FORWARD },
    // right
    { glm::vec3( .5,-.5, .5), glm::vec2(0,1), RIGHT },
    { glm::vec3( .5,-.5,-.5), glm::vec2(1,1), RIGHT },
    { glm::vec3( .5, .5,-.5), glm::vec2(1,0), RIGHT },
    { glm::vec3( .5, .5, .5), glm::vec2(0,0), RIGHT },
    // left
    { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -RIGHT },
    { glm::vec3(-.5,-.5, .5), glm::vec2(1,1), -RIGHT },
    { glm::vec3(-.5, .5, .5), glm::vec2(1,0), -RIGHT },
    { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), -RIGHT },
    // bottom
    { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -UP },
    { glm::vec3( .5,-.5,-.5), glm::vec2(1,1), -UP },
    { glm::vec3( .5,-.5, .5), glm::vec2(1,0), -UP },
    { glm::vec3(-.5,-.5, .5), glm::vec2(0,0), -UP },
    // top
    { glm::vec3(-.5, .5, .5), glm::vec2(0,1), UP },
    { glm::vec3( .5, .5, .5), glm::vec2(1,1), UP },
    { glm::vec3( .5, .5,-.5), glm::vec2(1,0), UP },
    { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), UP }
};

const std::vector<uint32_t> CubeGeometry::indices = {
    // front
    0, 1, 2,
    2, 3, 0,
    // right
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

size_t CubeGeometry::getVertexCount() const
{
    return vertices.size();
}

size_t CubeGeometry::getVertexBufferSize() const
{
    return sizeof(Vertex) * vertices.size();
}

size_t CubeGeometry::getIndexCount() const
{
    return indices.size();
}

const std::vector<Vertex>& CubeGeometry::getVertices() const
{
    return vertices;
}

size_t CubeGeometry::getIndexBufferSize() const
{
    return sizeof(uint32_t) * indices.size();
}

const std::vector<uint32_t>& CubeGeometry::getIndices() const
{
    return indices;
}

CubeGeometryUPtr CubeGeometry::create()
{
    return std::make_unique<CubeGeometry>();
}
