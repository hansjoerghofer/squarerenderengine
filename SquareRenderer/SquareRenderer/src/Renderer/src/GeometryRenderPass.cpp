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
	: BaseGeometryRenderPass(data.name, resources, matlib)
	, m_data(data)
{
}

GeometryRenderPass::~GeometryRenderPass()
{
}

void GeometryRenderPass::renderInternal(Renderer& renderer) const
{
	renderer.setTarget(m_data.target);

	if (m_data.thinGlassMode)
	{
		RendererState rs = m_data.state;
		rs.cullingMode = Culling::Front;
		renderer.applyState(rs);

		renderGeometry(renderer, m_data.drawables, m_data.overrideMaterial);
	}

	renderer.applyState(m_data.state);

	renderGeometry(renderer, m_data.drawables, m_data.overrideMaterial);
}

void GeometryRenderPass::setWireframeMode(bool flag)
{
	m_data.state.enableWireframe = flag;
}

bool GeometryRenderPass::wireframeMode() const
{
	return m_data.state.enableWireframe;
}