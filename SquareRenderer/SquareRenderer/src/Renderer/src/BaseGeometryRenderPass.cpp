#include "Renderer/BaseGeometryRenderPass.h"

#include "Renderer/IDrawable.h"
#include "Renderer/Renderer.h"
#include "Material/Material.h"

BaseGeometryRenderPass::BaseGeometryRenderPass(const std::string& name, ResourceManagerSPtr resources, MaterialLibrarySPtr matlib)
	: BaseRenderPass(name, resources, matlib)
{
}

BaseGeometryRenderPass::~BaseGeometryRenderPass()
{
}

void BaseGeometryRenderPass::renderGeometry(Renderer& renderer, const std::vector<IDrawableSPtr>& drawables) const
{
	for (const auto& elem : drawables)
	{
		elem->preRender(elem->material());
		renderer.render(elem->geometry(), elem->material());
		elem->postRender();
	}
}

void BaseGeometryRenderPass::renderGeometry(
	Renderer& renderer, 
	const std::vector<IDrawableSPtr>& drawables, 
	MaterialSPtr overrideMaterial) const
{
	if (!overrideMaterial)
	{
		renderGeometry(renderer, drawables);
		return;
	}

	for (const auto& elem : drawables)
	{
		// TODO handle material override

		elem->preRender(overrideMaterial);
		renderer.render(elem->geometry(), overrideMaterial);
		elem->postRender();
	}
}
