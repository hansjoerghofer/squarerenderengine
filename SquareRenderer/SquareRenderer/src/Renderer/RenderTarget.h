#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"
#include "Renderer/IRenderTarget.h"

#include <vector>

DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(DepthBuffer);
enum class DepthBufferFormat : int;

class RenderTarget : public IRenderTarget
{
public:

	RenderTarget(ITextureSPtr colorTarget,
		DepthBufferFormat format);

	RenderTarget(std::vector<ITextureSPtr>&& colorTargets,
		DepthBufferFormat format);

	explicit RenderTarget(ITextureSPtr colorTarget,
		DepthBufferSPtr depthBuffer = nullptr);

	RenderTarget(std::vector<ITextureSPtr>&& colorTargets,
		DepthBufferSPtr depthBuffer = nullptr);

	virtual int width() const override;

	virtual int height() const override;

	SharedResource::Handle handle() const override;

	const std::vector<ITextureSPtr>& colorTargets()const;

	DepthBufferSPtr depthBuffer() const;

	void link(IRenderTargetResourceUPtr resource);

protected:

	std::vector<ITextureSPtr> m_colorTargets;

	DepthBufferSPtr m_depthBuffer;

	IRenderTargetResourceUPtr m_linkedResource;
};

