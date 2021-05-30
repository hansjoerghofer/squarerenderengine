#include "Texture/Texture2D.h"

Texture2D::Texture2D(int width, int height, TextureFormat format,
	TextureSampler sampler)
	: m_width(width)
	, m_height(height)
	, m_format(format)
	, m_sampler(sampler)
	, m_isBound(false)
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::resize(int width, int height)
{
	if (m_width == width && 
		m_height == height)
	{
		return;
	}

	m_width = width;
	m_height = height;

	if (m_linkedResource)
	{
		bind();

		m_linkedResource->update(m_width, m_height, nullptr, false);

		unbind();
	}
}

TextureLayout Texture2D::layout() const
{
	return TextureLayout::Texture2D;
}

int Texture2D::width() const
{
	return m_width;
}

int Texture2D::height() const
{
	return m_height;
}

int Texture2D::layers() const
{
	return 1;
}

TextureFormat Texture2D::format() const
{
	return m_format;
}

const TextureSampler& Texture2D::sampler() const
{
	return m_sampler;
}

void Texture2D::link(ITextureResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

void Texture2D::bind()
{
	if (!m_isBound && m_linkedResource)
	{
		m_linkedResource->bind();
		m_isBound = true;
	}
}

void Texture2D::unbind()
{
	if (m_linkedResource)
	{
		m_linkedResource->unbind();
	}

	m_isBound = false;
}

SharedResource::Handle Texture2D::handle() const
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
