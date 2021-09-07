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
    
    constexpr static unsigned char DATA_POSITION    = 0;
    constexpr static unsigned char DATA_UV          = 1 << 0;
    constexpr static unsigned char DATA_NORMAL      = 1 << 1;
    constexpr static unsigned char DATA_TANGENT     = 1 << 2;

    constexpr static unsigned char DATA_FULL        = DATA_UV | DATA_NORMAL | DATA_TANGENT;
};

DECLARE_PTRS(IGeometry);

class IGeometry
{
public:

    virtual ~IGeometry() = 0 {};

    virtual void accept(IGeometryVisitor& visitor) = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual unsigned char dataFieldFlags() const = 0;
};