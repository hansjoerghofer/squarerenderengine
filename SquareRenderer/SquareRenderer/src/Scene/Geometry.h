#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"

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

    virtual ~Geometry() {};

    virtual void link(GeometryResourceUPtr resource);

    virtual void bind();

    virtual void unbind();

    virtual bool linked() const;

    virtual bool isBound() const;

    virtual bool isStatic() const = 0;

    virtual size_t vertexCount() const = 0;

    virtual size_t vertexBufferSize() const = 0;

    virtual size_t indexCount() const = 0;

    virtual size_t indexBufferSize() const = 0;

    virtual const std::vector<Vertex>& vertices() const = 0;

    virtual const std::vector<uint32_t>& indices() const = 0;

    virtual bool hasUVs() const = 0;

    virtual bool hasNormals() const = 0;

    virtual bool hasTangents() const = 0;

protected:

    GeometryResourceUPtr m_linkedResource;

    bool m_isBound = false;
};

class GeometryResource : public SharedResource
{
public:
    virtual ~GeometryResource() {};

    virtual void bind() = 0;

    virtual void unbind() = 0;
};
