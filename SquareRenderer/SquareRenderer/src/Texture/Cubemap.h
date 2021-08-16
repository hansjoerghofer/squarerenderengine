#pragma once

#include "Texture/ITexture.h"

#include "Common/Macros.h"

DECLARE_PTRS(Cubemap);

class Cubemap : public ITexture
{
public:

	Cubemap(int width, TextureFormat format,
		TextureSampler sampler = TextureSampler());

	virtual ~Cubemap();

	TextureFormat format() const;

	const TextureSampler& sampler() const;

	virtual TextureLayout layout() const override;

	virtual int width() const override;

	virtual int height() const override;

	virtual int layers() const override;

	virtual void bind() override;

	virtual void unbind() override;

	virtual void updateMipmaps();

	virtual SharedResource::Handle handle() const override;

	void link(ITextureResourceUPtr resource);

protected:

	int m_width;

	bool m_isBound;

	TextureFormat m_format;

	TextureSampler m_sampler;

	ITextureResourceUPtr m_linkedResource;
};

