#pragma once

#include "Common/Timer.h"

#include "Renderer/BaseRenderPass.h"
#include "Renderer/RendererState.h"

DECLARE_PTRS(IDrawable);
DECLARE_PTRS(GeometryRenderPass);

class GeometryRenderPass : public BaseRenderPass
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

    GeometryRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib, const Data& command);

    virtual ~GeometryRenderPass();

    virtual bool isEnabled() const override;

    virtual void setEnabled(bool flag) override;

    void setWireframeMode(bool flag);

    bool wireframeMode() const;

protected:

    virtual void renderInternal(Renderer& renderer) const override;

    Data m_data;
};