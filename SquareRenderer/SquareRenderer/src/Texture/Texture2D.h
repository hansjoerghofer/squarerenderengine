#pragma once

#include "Texture/ITexture.h"

#include <vector>

DECLARE_PTRS(Texture2D);

class Texture2D : public ITexture
{
public:

	Texture2D(int width, int height, TextureFormat format, 
		TextureSampler sampler = TextureSampler());
	
	virtual ~Texture2D();

	void resize(int width, int height);

	virtual TextureLayout layout() const override;

	virtual int width() const override;

	virtual int height() const override;

	virtual int layers() const override;

	virtual SharedResource::Handle handle() const override;

	virtual TextureFormat format() const;

	const TextureSampler& sampler() const;

	void link(ITextureResourceUPtr resource);

private:

	int m_width;

	int m_height;

	TextureFormat m_format;

	TextureSampler m_sampler;

	ITextureResourceUPtr m_linkedResource;
};

