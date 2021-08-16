#include "Texture/Cubemap.h"

Cubemap::Cubemap(int width, TextureFormat format, TextureSampler sampler)
    : m_width(width)
    , m_format(format)
    , m_sampler(sampler)
    , m_isBound(false)
{
}

Cubemap::~Cubemap()
{
}

TextureFormat Cubemap::format() const
{
    return m_format;
}

const TextureSampler& Cubemap::sampler() const
{
    return m_sampler;
}

TextureLayout Cubemap::layout() const
{
    return TextureLayout::Cubemap;
}

int Cubemap::width() const
{
    return m_width;
}

int Cubemap::height() const
{
    return m_width;
}

int Cubemap::layers() const
{
    return 6;
}

void Cubemap::bind()
{
    if (!m_isBound && m_linkedResource)
    {
        m_linkedResource->bind();
        m_isBound = true;
    }
}

void Cubemap::unbind()
{
    if (m_linkedResource)
    {
        m_linkedResource->unbind();
    }

    m_isBound = false;
}

void Cubemap::updateMipmaps()
{
    if (!m_linkedResource)
    {
        return;
    }

    bool wasBound = m_isBound;
    if (!m_isBound)
    {
        bind();
    }

    m_linkedResource->generateMipmaps();

    if (!wasBound)
    {
        unbind();
    }
}

SharedResource::Handle Cubemap::handle() const
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

void Cubemap::link(ITextureResourceUPtr resource)
{
    m_linkedResource = std::move(resource);
}
