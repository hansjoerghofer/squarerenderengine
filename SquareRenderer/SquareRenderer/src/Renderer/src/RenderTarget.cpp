#include "Renderer/RenderTarget.h"
#include "Renderer/DepthBuffer.h"
#include "Texture/ITexture.h"
#include "Texture/Texture2D.h"

RenderTarget::RenderTarget(
	ITextureSPtr colorTarget, DepthBufferFormat format)
	: m_colorTargets({ colorTarget })
	, m_depthBuffer(new DepthBuffer(colorTarget->width(), colorTarget->height(), format))
{
}

RenderTarget::RenderTarget(
	std::vector<ITextureSPtr>&& colorTargets, DepthBufferFormat format)
	: m_colorTargets(std::move(colorTargets))
	, m_depthBuffer(new DepthBuffer(
		m_colorTargets.front()->width(), 
		m_colorTargets.front()->height(), format))
{
}

RenderTarget::RenderTarget(
	ITextureSPtr colorTarget, IDepthAttachmentSPtr depthBuffer)
	: m_colorTargets({ colorTarget })
	, m_depthBuffer(depthBuffer)
{
}

RenderTarget::RenderTarget(
	std::vector<ITextureSPtr>&& colorTargets, IDepthAttachmentSPtr depthBuffer)
	: m_colorTargets(std::move(colorTargets))
	, m_depthBuffer(depthBuffer)
{
}

RenderTarget::RenderTarget(IDepthAttachmentSPtr depthBuffer)
	: m_colorTargets()
	, m_depthBuffer(depthBuffer)
{
}

int RenderTarget::width() const
{
	if (!m_colorTargets.empty())
	{
		return m_colorTargets.front()->width();
	}
	else if (m_depthBuffer)
	{
		return m_depthBuffer->width();
	}
	return 0;
}

int RenderTarget::height() const
{
	if (!m_colorTargets.empty())
	{
		return m_colorTargets.front()->height();
	}
	else if (m_depthBuffer)
	{
		return m_depthBuffer->height();
	}
	return 0;
}

SharedResource::Handle RenderTarget::handle() const
{
	if (m_linkedResource)
	{
		return m_linkedResource->handle();
	}
	else
	{
		return SharedResource::INVALID_HANDLE;
	}
}

const std::vector<ITextureSPtr>& RenderTarget::colorTargets() const
{
	return m_colorTargets;
}

IDepthAttachmentSPtr RenderTarget::depthBuffer() const
{
	return m_depthBuffer;
}

void RenderTarget::link(IRenderTargetResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}
