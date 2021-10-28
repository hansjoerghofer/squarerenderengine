#include "Renderer/ResourceManager.h"
#include "API/GraphicsAPI.h"
#include "Common/Logger.h"
#include "Renderer/IRenderTarget.h"
#include "Renderer/RenderTarget.h"
#include "Scene/IGeometry.h"
#include "Scene/MeshBuilder.h"
#include "Texture/Texture2D.h"

ResourceManager::ResourceManager(GraphicsAPISPtr api)
	: m_api(api)
{
	m_fullscreenGeometry = MeshBuilder::screenTriangle();
	if (!m_api->allocate(m_fullscreenGeometry))
	{
		Logger::Warning("Error while allocating fullscreen triangle buffer.");
	}
}

ResourceManager::~ResourceManager()
{
}

IGeometrySPtr ResourceManager::fullscreenGeometry() const
{
	return m_fullscreenGeometry;
}

bool ResourceManager::allocateRenderTarget(RenderTargetSPtr rt)
{
	return m_api->allocate(rt);
}

RenderTargetSPtr ResourceManager::createSimpleColorTarget(IRenderTargetSPtr source, float scale, TextureFormat format, TextureSampler sampler)
{
	return createSimpleColorTarget(static_cast<int>(source->width() * scale), static_cast<int>(source->height() * scale), format, sampler);
}

RenderTargetSPtr ResourceManager::createSimpleColorTarget(int width, int height, TextureFormat format, TextureSampler sampler)
{
	RenderTargetSPtr rt = std::make_shared<RenderTarget>(
		std::make_shared<Texture2D>(
			width, 
			height, 
			format, 
			sampler));

	if (m_api->allocate(rt))
	{
		return rt;
	}

	return RenderTargetSPtr();
}
