#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"
#include "Renderer/IRenderTarget.h"

#include <vector>

DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IDepthAttachment);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(Texture2D);
enum class DepthBufferFormat : int;

class RenderTarget : public IRenderTarget
{
public:

	RenderTarget(ITextureSPtr colorTarget,
		DepthBufferFormat format, int level = 0);

	RenderTarget(std::vector<ITextureSPtr>&& colorTargets,
		DepthBufferFormat format, int level = 0);

	explicit RenderTarget(ITextureSPtr colorTarget,
		IDepthAttachmentSPtr depthBuffer = nullptr, int level = 0);

	explicit RenderTarget(std::vector<ITextureSPtr>&& colorTargets,
		IDepthAttachmentSPtr depthBuffer = nullptr, int level = 0);

	explicit RenderTarget(IDepthAttachmentSPtr depthBuffer);

	virtual int width() const override;

	virtual int height() const override;

	virtual glm::vec4 dimensions() const override;

	virtual int level() const;

	SharedResource::Handle handle() const override;

	const std::vector<ITextureSPtr>& colorTargets() const;

	ITextureSPtr colorTarget(size_t index = 0) const;

	template<typename T>
	std::shared_ptr<T> colorTargetAs(size_t index = 0) const
	{
		return std::static_pointer_cast<T>(colorTarget(index));
	}

	IDepthAttachmentSPtr depthBuffer() const;

	template<typename T>
	std::shared_ptr<T> depthBufferAs() const
	{
		return std::static_pointer_cast<T>(depthBuffer());
	}

	void link(IRenderTargetResourceUPtr resource);

protected:

	std::vector<ITextureSPtr> m_colorTargets;

	IDepthAttachmentSPtr m_depthBuffer;

	IRenderTargetResourceUPtr m_linkedResource;

	int m_level;
};

