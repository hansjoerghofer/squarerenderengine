#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"
#include "Scene/IGeometryVisitor.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;

    glm::vec3 normal;
    glm::vec3 tangent;
};

DECLARE_PTRS(IGeometry);

class IGeometry
{
public:

    virtual void accept(IGeometryVisitor& visitor) = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;
};