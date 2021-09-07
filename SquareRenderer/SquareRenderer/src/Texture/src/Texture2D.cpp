#include "Texture/Texture2D.h"

Texture2D::Texture2D(int width, int height, TextureFormat format,
	TextureSampler sampler)
	: m_width(width)
	, m_height(height)
	, m_format(format)
	, m_sampler(sampler)
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
		m_linkedResource->update(m_width, m_height, nullptr);
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

void Texture2D::update(int width, int height, TextureFormat format)
{
	m_width = width;
	m_height = height;
	m_format = format;

	m_linkedResource.reset();
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
