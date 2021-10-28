#pragma once
#include "Renderer/BaseRenderPass.h"

DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(Texture2D);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(BloomRenderPass);

class BloomRenderPass : public BaseRenderPass
{
public:

	BloomRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib);

	virtual ~BloomRenderPass();

	void setup(IRenderTargetSPtr target, Texture2DSPtr screenBuffer);

	virtual void update(double deltaTime) override;

	Texture2DSPtr blurBuffer() const;

	int iterations() const;

	void setIterations(int value);

	float threshold() const;

	void setThreshold(float value);

	float intensity() const;

	void setIntensity(float value);

protected:

	void renderInternal(Renderer& renderer) const override;

	int m_numBlurIterations = 1;

	float m_scale = .125f;

	float m_brightnessThreshold = 2.f;

	float m_intensity = 0.25f;

	MaterialSPtr m_downsampling;
	MaterialSPtr m_brightpassFilter;
	MaterialSPtr m_verticalBlur;
	MaterialSPtr m_horizontalBlur;
	MaterialSPtr m_blendAdd;

	Texture2DSPtr m_sourceBuffer;

	RenderTargetSPtr m_highIntensityBuffer;
	RenderTargetSPtr m_pingRT;
	RenderTargetSPtr m_pongRT;
};

