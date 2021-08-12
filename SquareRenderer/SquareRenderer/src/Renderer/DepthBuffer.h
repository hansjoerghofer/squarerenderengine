#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"

DECLARE_PTRS(DepthBuffer);
DECLARE_PTRS(DepthTextureWrapper);
DECLARE_PTRS(Texture2D);

enum class DepthBufferFormat
{
	Depth16,
	Depth24,
	DepthFloat,
	Depth24Stencil8,
	DepthFloatStencil8
};

enum class DepthAttachmentType
{
	Texture2D,
	Renderbuffer
};

class IDepthAttachment
{
public:
	virtual int width() const = 0;

	virtual int height() const = 0;

	virtual DepthAttachmentType attachmentType() const = 0;

	virtual bool hasStencilBuffer() const = 0;

	virtual SharedResource::Handle handle() const = 0;
};

class DepthBuffer : public IDepthAttachment
{
public:

	DepthBuffer(int width, int height, DepthBufferFormat format);

	virtual DepthAttachmentType attachmentType() const override;

	virtual bool hasStencilBuffer() const override;

	virtual int width() const override;

	virtual int height() const override;

	DepthBufferFormat format() const;

	void link(IDepthBufferResourceUPtr resource);

	virtual SharedResource::Handle handle() const override;

private:

	int m_width;
	
	int m_height;

	DepthBufferFormat m_format;

	IDepthBufferResourceUPtr m_linkedResource;
};

class DepthTextureWrapper : public IDepthAttachment
{
public:
	DepthTextureWrapper(Texture2DSPtr texture);

	virtual int width() const override;

	virtual int height() const override;

	virtual DepthAttachmentType attachmentType() const override;

	virtual bool hasStencilBuffer() const override;

	virtual SharedResource::Handle handle() const override;

	Texture2DSPtr texture() const;

private:

	Texture2DSPtr m_texture;
};

