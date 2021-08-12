#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include "API/SharedResource.h"
#include "Renderer/IGeometry.h"

#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;

    glm::vec3 normal;
    glm::vec3 tangent;
};

DECLARE_PTRS(Geometry);

class Geometry : public IGeometry
{
public:

    virtual ~Geometry() {};

    virtual void link(IGeometryResourceUPtr resource);

    virtual void bind() override;

    virtual void unbind() override;

    virtual bool linked() const;

    virtual bool isBound() const;

    virtual bool isStatic() const = 0;

    virtual size_t vertexBufferSize() const = 0;

    virtual size_t indexBufferSize() const = 0;

    virtual const std::vector<Vertex>& vertices() const = 0;

    virtual const std::vector<uint32_t>& indices() const = 0;

    virtual bool hasUVs() const = 0;

    virtual bool hasNormals() const = 0;

    virtual bool hasTangents() const = 0;

protected:

    IGeometryResourceUPtr m_linkedResource;

    bool m_isBound = false;
};
