#include "Renderer/RenderTarget.h"
#include "Renderer/DepthBuffer.h"
#include "Texture/ITexture.h"
#include "Texture/Texture2D.h"

RenderTarget::RenderTarget(
	ITextureSPtr colorTarget, DepthBufferFormat format, int level)
	: m_colorTargets({ colorTarget })
	, m_depthBuffer(new DepthBuffer(colorTarget->width(), colorTarget->height(), format))
	, m_level(level)
{
}

RenderTarget::RenderTarget(
	std::vector<ITextureSPtr>&& colorTargets, DepthBufferFormat format, int level)
	: m_colorTargets(std::move(colorTargets))
	, m_depthBuffer(new DepthBuffer(
		m_colorTargets.front()->width(), 
		m_colorTargets.front()->height(), format))
	, m_level(level)
{
}

RenderTarget::RenderTarget(
	ITextureSPtr colorTarget, IDepthAttachmentSPtr depthBuffer, int level)
	: m_colorTargets({ colorTarget })
	, m_depthBuffer(depthBuffer)
	, m_level(level)
{
}

RenderTarget::RenderTarget(
	std::vector<ITextureSPtr>&& colorTargets, IDepthAttachmentSPtr depthBuffer, int level)
	: m_colorTargets(std::move(colorTargets))
	, m_depthBuffer(depthBuffer)
	, m_level(level)
{
}

RenderTarget::RenderTarget(IDepthAttachmentSPtr depthBuffer)
	: m_colorTargets()
	, m_depthBuffer(depthBuffer)
	, m_level(0)
{
}

int RenderTarget::width() const
{
	int width = 0;

	if (!m_colorTargets.empty())
	{
		width = m_colorTargets.front()->width();
	}
	else if (m_depthBuffer)
	{
		width = m_depthBuffer->width();
	}

	// compute the size of the selected level
	return static_cast<int>(width * std::pow(0.5, m_level));
}

int RenderTarget::height() const
{
	int height = 0;

	if (!m_colorTargets.empty())
	{
		height = m_colorTargets.front()->height();
	}
	else if (m_depthBuffer)
	{
		height = m_depthBuffer->height();
	}

	// compute the size of the selected level
	return static_cast<int>(height * std::pow(0.5, m_level));
}

glm::vec4 RenderTarget::dimensions() const
{
	return glm::vec4(width(), height(), 1.f / width(), 1.f / height());
}

int RenderTarget::level() const
{
	return m_level;
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

ITextureSPtr RenderTarget::colorTarget(size_t index) const
{
	if (index < colorTargets().size())
	{
		return colorTargets()[index];
	}
	return ITextureSPtr();
}

IDepthAttachmentSPtr RenderTarget::depthBuffer() const
{
	return m_depthBuffer;
}

void RenderTarget::link(IRenderTargetResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}
