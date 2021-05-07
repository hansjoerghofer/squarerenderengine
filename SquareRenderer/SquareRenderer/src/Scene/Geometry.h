#pragma once

#include "Common/Macros.h"
#include "Resources/SharedResource.h"

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;

    glm::vec3 normal;
    glm::vec3 tangent;
};

DECLARE_PTRS(GeometryResource);
DECLARE_PTRS(Geometry);

class Geometry
{
public:

    constexpr static const int INVALID_HANDLE = -1;

    virtual ~Geometry() {};

    virtual void link(GeometryResourceUPtr resource) = 0;

    virtual bool hasLink() const = 0;

    virtual bool isStatic() const = 0;

    virtual size_t vertexCount() const = 0;

    virtual size_t vertexBufferSize() const = 0;

    virtual size_t indexCount() const = 0;

    virtual size_t indexBufferSize() const = 0;

    virtual const std::vector<Vertex>& vertices() const = 0;

    virtual const std::vector<uint32_t>& indices() const = 0;

protected:

    GeometryResourceUPtr m_linkedResource;
};

class GeometryResource : public SharedResource
{
public:
    virtual ~GeometryResource() {};
};
