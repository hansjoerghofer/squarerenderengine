#pragma once
#include "IRenderPass.h"
#include "RendererState.h"

DECLARE_PTRS(Material);
DECLARE_PTRS(IGeometry);

class ScreenSpaceRenderPass : public IRenderPass
{
public:

	ScreenSpaceRenderPass(
		const std::string& name, 
		IGeometrySPtr geometry, 
		MaterialSPtr material, 
		IRenderTargetSPtr target,
		RendererState state = RendererState::Blit());

	virtual ~ScreenSpaceRenderPass();

	virtual const std::string& name() const override;

	virtual IRenderTargetSPtr target() const override;

	virtual bool isEnabled() const override;

	virtual void setEnabled(bool flag) override;

	virtual void render(Renderer& renderer) const override;

	virtual void update(double deltaTime) override;

protected:

	std::string m_name;

	bool m_enabled = true;

	IRenderTargetSPtr m_target;

	MaterialSPtr m_material;

	IGeometrySPtr m_fullscreenTriangle;

	RendererState m_state;
};

