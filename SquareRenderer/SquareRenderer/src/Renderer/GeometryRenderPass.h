#pragma once

#include "Renderer/IRenderPass.h"
#include "Renderer/RendererState.h"

#include <vector>

struct RendererState;
DECLARE_PTRS(Material);
DECLARE_PTRS(Renderer);
DECLARE_PTRS(IDrawable);
DECLARE_PTRS(GeometryRenderPass);

class GeometryRenderPass : public IRenderPass
{
public:

    struct Data
    {
        bool enabled = true;

        std::string name;

        RendererState state;

        std::vector<IDrawableSPtr> drawables;

        IRenderTargetSPtr target = nullptr;

        MaterialSPtr overrideMaterial = nullptr;
    };

    GeometryRenderPass(const Data& command);

    GeometryRenderPass(Data&& command);

    virtual ~GeometryRenderPass();

    const std::string& name() const override;

    virtual IRenderTargetSPtr target() const override;

    virtual void render(Renderer& renderer) const override;

    virtual void update(double deltaTime) override;

    virtual bool isEnabled() const override;

    virtual void setEnabled(bool flag) override;

    void setWireframeMode(bool flag);

    bool wireframeMode() const;

protected:

    Data m_data;
};