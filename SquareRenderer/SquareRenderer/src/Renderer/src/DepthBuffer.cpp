#include "Renderer/DepthBuffer.h"
#include "Texture/Texture2D.h"

DepthBuffer::DepthBuffer(int width, int height, DepthBufferFormat format)
	: m_width(width)
	, m_height(height)
	, m_format(format)
{
}

DepthAttachmentType DepthBuffer::attachmentType() const
{
	return DepthAttachmentType::Renderbuffer;
}

bool DepthBuffer::hasStencilBuffer() const
{
	return m_format == DepthBufferFormat::Depth24Stencil8 ||
		   m_format == DepthBufferFormat::DepthFloatStencil8;
}

int DepthBuffer::width() const
{
	return m_width;
}

int DepthBuffer::height() const
{
	return m_height;
}

DepthBufferFormat DepthBuffer::format() const
{
	return m_format;
}

void DepthBuffer::link(IDepthBufferResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

SharedResource::Handle DepthBuffer::handle() const
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

DepthTextureWrapper::DepthTextureWrapper(Texture2DSPtr texture)
	: m_texture(texture)
{
	ASSERT(texture->format() == TextureFormat::DepthHalf ||
		texture->format() == TextureFormat::DepthFloat);
}

int DepthTextureWrapper::width() const
{
	return m_texture->width();
}

int DepthTextureWrapper::height() const
{
	return m_texture->height();
}

DepthAttachmentType DepthTextureWrapper::attachmentType() const
{
	return DepthAttachmentType::Texture2D;
}

bool DepthTextureWrapper::hasStencilBuffer() const
{
	return false;
}

SharedResource::Handle DepthTextureWrapper::handle() const
{
	return m_texture->handle();
}

Texture2DSPtr DepthTextureWrapper::texture() const
{
	return m_texture;
}
