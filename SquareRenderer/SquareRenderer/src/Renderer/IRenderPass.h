#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(IRenderPass);
DECLARE_PTRS(Renderer);

struct RenderStatisticsData
{
    double cpuTimeMs = 0.0;
    double gpuTimeMs = 0.0;

    size_t rendererdPrimitives = 0;
};

class IRenderPass
{
public:

    virtual ~IRenderPass() = 0 {};

    virtual const std::string& name() const = 0;

    virtual IRenderTargetSPtr target() const = 0;

    virtual bool isEnabled() const = 0;

    virtual void setEnabled(bool flag) = 0;

    virtual void render(Renderer& renderer) const = 0;

    virtual void update(double deltaTime) = 0;

    virtual RenderStatisticsData renderStatistics() const = 0;
};