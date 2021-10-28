#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"
#include "Renderer/BaseRenderPass.h"
#include "Renderer/RendererState.h"

DECLARE_PTRS(Scene);
DECLARE_PTRS(IDrawable);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(ILightsource);
DECLARE_PTRS(Material);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(ShadowMappingRenderPass);

struct ShadowData
{
	int index = -1;
	glm::mat4 lightMatrice = glm::mat4(1);

	RenderTargetSPtr target;
	MaterialSPtr material;
};

class ShadowMappingRenderPass : public BaseRenderPass
{
public:
	ShadowMappingRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib);

	virtual ~ShadowMappingRenderPass();

	void setup(SceneSPtr scene);

	virtual void update(double deltaTime) override;

	const std::unordered_map<ILightsourceSPtr, ShadowData>& shadowData() const;

private:
	virtual void renderInternal(Renderer& renderer) const override;

	RendererState m_state;

	SceneSPtr m_scene;

	std::vector<IDrawableSPtr> m_geometry;

	std::unordered_map<ILightsourceSPtr, ShadowData> m_shadowData;
};

