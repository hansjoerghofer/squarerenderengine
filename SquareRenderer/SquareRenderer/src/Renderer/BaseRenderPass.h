#pragma once

#include "Common/Macros.h"
#include "Common/Timer.h"
#include "Renderer/IRenderPass.h"
#include "Renderer/RendererState.h"

DECLARE_PTRS(GPUTimer);
DECLARE_PTRS(ResourceManager);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(Material);

class BaseRenderPass : public IRenderPass
{
public:
	BaseRenderPass(const std::string& name, ResourceManagerSPtr resources, MaterialLibrarySPtr matlib);

	virtual ~BaseRenderPass();

	virtual const std::string& name() const override;

	void render(Renderer& renderer) const override final;
	
	virtual void update(double deltaTime) override;

	virtual void setup(IRenderTargetSPtr target);

	virtual bool isEnabled() const override;

	virtual void setEnabled(bool flag) override;

	virtual IRenderTargetSPtr target() const override;

	virtual RenderStatisticsData renderStatistics() const override;

protected:

	virtual void renderInternal(Renderer& renderer) const = 0;

	void blit(Renderer& renderer, IRenderTargetSPtr target, MaterialSPtr material, const RendererState& state = RendererState::Blit()) const;
	
	bool m_enabled = true;

	std::string m_name;

	IRenderTargetSPtr m_target;

	ResourceManagerSPtr m_resources;

	MaterialLibrarySPtr m_matlib;

	mutable RenderStatisticsData m_renderStatistics;

	mutable Timer m_timer;
	mutable GPUTimerUPtr m_gpuTimer;
};

