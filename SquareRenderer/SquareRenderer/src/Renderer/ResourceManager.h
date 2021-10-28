#pragma once

#include "Common/Macros.h"
#include "Texture/TextureDefines.h"

DECLARE_PTRS(ResourceManager);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(RenderTarget);

class ResourceManager
{
public:
	ResourceManager(GraphicsAPISPtr api);

	~ResourceManager();

	IGeometrySPtr fullscreenGeometry() const;

	bool allocateRenderTarget(RenderTargetSPtr source);

	RenderTargetSPtr createSimpleColorTarget(IRenderTargetSPtr source, float scale = 1.f,
		TextureFormat format = TextureFormat::RGBAHalf,
		TextureSampler sampler = { TextureFilter::Linear, TextureWrap::Mirror, false, glm::vec4(0) });

	RenderTargetSPtr createSimpleColorTarget(int width, int height,
		TextureFormat format = TextureFormat::RGBAHalf, 
		TextureSampler sampler = { TextureFilter::Linear, TextureWrap::Mirror, false, glm::vec4(0) });

private:

	GraphicsAPISPtr m_api;

	IGeometrySPtr m_fullscreenGeometry;
};

