#include "Renderer/GeometryRenderPass.h"
#include "API/GraphicsAPI.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Renderer/IDrawable.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/ResourceManager.h"
#include "Scene/IGeometry.h"
#include "Texture/Texture2D.h"
#include "Texture/TextureDefines.h"

GeometryRenderPass::GeometryRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib, const Data& data)
	: BaseRenderPass(data.name, resources, matlib)
	, m_data(data)
{
}

GeometryRenderPass::~GeometryRenderPass()
{
}

void GeometryRenderPass::renderInternal(Renderer& renderer) const
{
	renderer.setTarget(m_data.target);

	renderer.applyState(m_data.state);

	for (const auto& elem : m_data.drawables)
	{
		// TODO override logic!
		MaterialSPtr mat = m_data.overrideMaterial ? m_data.overrideMaterial : elem->material();
		if (mat)
		{
			elem->preRender(mat);
			renderer.render(elem->geometry(), mat);
			elem->postRender();
		}
	}
}

bool GeometryRenderPass::isEnabled() const
{
	return m_data.enabled;
}

void GeometryRenderPass::setEnabled(bool flag)
{
	m_data.enabled = flag;
}

void GeometryRenderPass::setWireframeMode(bool flag)
{
	m_data.state.enableWireframe = flag;
}

bool GeometryRenderPass::wireframeMode() const
{
	return m_data.state.enableWireframe;
}