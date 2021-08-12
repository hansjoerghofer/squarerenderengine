#pragma once

#include "Common/Macros.h"
#include "Renderer/RenderCommand.h"

#include <string>
#include <vector>

struct RendererState;
DECLARE_PTRS(Material);
DECLARE_PTRS(IDrawable);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(RenderPass);

class RenderPass
{
public:

    RenderPass(RenderCommand&& command);

    virtual ~RenderPass();

    std::string name() const;

    bool isEnabled() const;

    void setEnabled(bool flag);

    const RendererState& rendererState() const;

    IRenderTargetSPtr target() const;

    MaterialSPtr material() const;

    const std::vector<IDrawableSPtr>& drawables() const;

    virtual void update(double deltaTime);

    virtual void preRender();


protected:
    RenderCommand m_command;
};

