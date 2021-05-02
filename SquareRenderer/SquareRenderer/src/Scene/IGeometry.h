#pragma once

#include <glm/glm.hpp>

#include <vector>

constexpr glm::vec3 FORWARD =   glm::vec3(0, 0, 1);
constexpr glm::vec3 RIGHT =     glm::vec3(1, 0, 0);
constexpr glm::vec3 UP =        glm::vec3(0, 1, 0);

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;

    glm::vec3 normal;
    glm::vec3 tangent;
};

class IGeometry
{
public:

    virtual ~IGeometry() {};

    virtual size_t getVertexCount() const = 0;

    virtual size_t getVertexBufferSize() const = 0;

    virtual size_t getIndexCount() const = 0;

    virtual size_t getIndexBufferSize() const = 0;

    virtual const std::vector<Vertex>& getVertices() const = 0;

    virtual const std::vector<uint32_t>& getIndices() const = 0;
};
