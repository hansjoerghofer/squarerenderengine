#pragma once

#include "Renderer/BaseRenderPass.h"

DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(Texture2D);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(TonemappingRenderPass);

class TonemappingRenderPass : public BaseRenderPass
{
public:

	TonemappingRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib);

	virtual ~TonemappingRenderPass();

	virtual void setup(IRenderTargetSPtr target, RenderTargetSPtr input);

	virtual void setEnabled(bool flag) override;

private:
	virtual void renderInternal(Renderer& renderer) const override;

	RenderTargetSPtr m_input;

	MaterialSPtr m_tonemapping;
	MaterialSPtr m_passthrough;
};

