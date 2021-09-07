#pragma once
#include "Renderer/IRenderPass.h"

DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(IGeometry);

class BloomRenderPass : public IRenderPass
{
public:

	BloomRenderPass(GraphicsAPISPtr api, MaterialLibrarySPtr matlib, IGeometrySPtr fullscreenTriangle, IRenderTargetSPtr target);

	~BloomRenderPass();

	void setup();

	virtual const std::string& name() const override;

	virtual IRenderTargetSPtr target() const override;

	virtual bool isEnabled() const override;

	virtual void setEnabled(bool flag) override;

	virtual void render(Renderer& renderer) const override;

	virtual void update(double deltaTime) override;

protected:

	bool m_enabled = true;

	int m_numBlurIterations = 3;

	float m_scale = .2f;

	float m_brightnessThreshold = 1.f;

	GraphicsAPISPtr m_api;
	MaterialLibrarySPtr m_matlib;

	IRenderTargetSPtr m_target;

	IGeometrySPtr m_fullscreenTriangle;

	RenderTargetSPtr m_upsampledRT;

	RenderTargetSPtr m_pingRT;
	RenderTargetSPtr m_pongRT;

	IRenderPassUPtr m_highpassPass;
	IRenderPassUPtr m_verticalBlur0;
	IRenderPassUPtr m_verticalBlurIt;
	IRenderPassUPtr m_horizontalBlurIt;
	IRenderPassUPtr m_blendPass;
};

