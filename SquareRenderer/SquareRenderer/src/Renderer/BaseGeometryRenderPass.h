#pragma once

#include "Renderer/BaseRenderPass.h"

DECLARE_PTRS(IDrawable);

class BaseGeometryRenderPass : public BaseRenderPass
{
public:

	BaseGeometryRenderPass(const std::string& name, ResourceManagerSPtr resources, MaterialLibrarySPtr matlib);

	virtual ~BaseGeometryRenderPass();

protected:

	virtual void renderGeometry(
		Renderer& renderer, 
		const std::vector<IDrawableSPtr>& drawables) const;

	virtual void renderGeometry(
		Renderer& renderer,
		const std::vector<IDrawableSPtr>& drawables,
		MaterialSPtr overrideMaterial) const;
};

