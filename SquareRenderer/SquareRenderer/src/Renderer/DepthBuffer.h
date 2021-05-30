#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"

DECLARE_PTRS(DepthBuffer);

enum class DepthBufferFormat
{
	Depth16,
	Depth24,
	DepthFloat,
	Depth24Stencil8,
	DepthFloatStencil8
};

class DepthBuffer
{
public:

	DepthBuffer(int width, int height, DepthBufferFormat format);

	int width() const;

	int height() const;

	DepthBufferFormat format() const;

	void link(IDepthBufferResourceUPtr resource);

	SharedResource::Handle handle() const;

private:

	int m_width;
	
	int m_height;

	DepthBufferFormat m_format;

	IDepthBufferResourceUPtr m_linkedResource;
};

