#pragma once
#include "Renderer/IRenderPass.h"

DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(BloomRenderPass);

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

	ITextureSPtr blurBuffer() const;

	int iterations() const;

	void setIterations(int value);

	float threshold() const;

	void setThreshold(float value);

protected:

	RenderTargetSPtr createColorBuffer(int width, int height);

	std::string m_name = "Bloom";

	bool m_enabled = true;

	int m_numBlurIterations = 1;

	float m_scale = .25f;

	float m_brightnessThreshold = 1.f;

	GraphicsAPISPtr m_api;
	MaterialLibrarySPtr m_matlib;

	IRenderTargetSPtr m_target;

	IGeometrySPtr m_fullscreenTriangle;

	MaterialSPtr m_brightpassFilter;

	RenderTargetSPtr m_pingRT;
	RenderTargetSPtr m_pongRT;

	IRenderPassUPtr m_highpassPass;
	IRenderPassUPtr m_verticalBlur0;
	IRenderPassUPtr m_verticalBlurIt;
	IRenderPassUPtr m_horizontalBlurIt;
	IRenderPassUPtr m_blendPass;
};

