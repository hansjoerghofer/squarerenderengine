#include "Renderer/DepthBuffer.h"

DepthBuffer::DepthBuffer(int width, int height, DepthBufferFormat format)
	: m_width(width)
	, m_height(height)
	, m_format(format)
{
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

