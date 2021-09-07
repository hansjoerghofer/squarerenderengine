#include "Renderer/GeometryRenderPass.h"

#include "Renderer/IDrawable.h"
#include "Renderer/IRenderTarget.h"
#include "Renderer/Renderer.h"
#include "Material/Material.h"
#include "Scene/IGeometry.h"

GeometryRenderPass::GeometryRenderPass(const Data& data)
	: m_data(data)
{
}

GeometryRenderPass::GeometryRenderPass(Data&& data)
	: m_data(data)
{
}

GeometryRenderPass::~GeometryRenderPass()
{
}

const std::string& GeometryRenderPass::name() const
{
	return m_data.name;
}

IRenderTargetSPtr GeometryRenderPass::target() const
{
	return m_data.target;
}

void GeometryRenderPass::render(Renderer& renderer) const
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

void GeometryRenderPass::update(double /*deltaTime*/)
{
}

bool GeometryRenderPass::isEnabled() const
{
	return m_data.enabled;
}

void GeometryRenderPass::setEnabled(bool flag)
{
	m_data.enabled = flag;
}
