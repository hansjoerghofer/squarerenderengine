#pragma once

#include "Renderer/BaseRenderPass.h"

DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(ResourceManager);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(Texture2D);
DECLARE_PTRS(SSAORenderPass);

class SSAORenderPass : public BaseRenderPass
{
public:

	SSAORenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib);

	virtual ~SSAORenderPass();

	void setup(IRenderTargetSPtr target, Texture2DSPtr screenBuffer, Texture2DSPtr normalsBuffer);

protected:

	virtual void renderInternal(Renderer& renderer) const override;

	RenderTargetSPtr m_aoBuffer;

	MaterialSPtr m_aoMaterial;
	MaterialSPtr m_blitMaterial;
};

