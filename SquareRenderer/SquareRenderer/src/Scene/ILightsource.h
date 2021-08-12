#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

DECLARE_PTRS(ILightsource);

enum class LightsourceType
{
    Directional,
    Point
};

class ILightsource
{
public:
    virtual LightsourceType type() const = 0;

    virtual const glm::vec3& color() const = 0;

    virtual float intensity() const = 0;

    virtual bool isShadowCaster() const = 0;
};

