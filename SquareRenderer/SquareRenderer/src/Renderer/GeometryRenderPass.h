#pragma once

#include "Common/Timer.h"

#include "Renderer/BaseGeometryRenderPass.h"
#include "Renderer/RendererState.h"

DECLARE_PTRS(IDrawable);
DECLARE_PTRS(GeometryRenderPass);

class GeometryRenderPass : public BaseGeometryRenderPass
{
public:

    struct Data
    {
        std::string name;

        RendererState state;

        std::vector<IDrawableSPtr> drawables;

        IRenderTargetSPtr target = nullptr;

        MaterialSPtr overrideMaterial = nullptr;

        bool thinGlassMode = false;
    };

    GeometryRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib, const Data& command);

    virtual ~GeometryRenderPass();

    void setWireframeMode(bool flag);

    bool wireframeMode() const;

protected:

    virtual void renderInternal(Renderer& renderer) const override;

    Data m_data;
};