#include "Renderer/ScreenSpaceRenderPass.h"
#include "Renderer/Renderer.h"

ScreenSpaceRenderPass::ScreenSpaceRenderPass(
	const std::string& name, 
	IGeometrySPtr geometry, 
	MaterialSPtr material, 
	IRenderTargetSPtr target,
	RendererState state)
	: m_name(name)
	, m_fullscreenTriangle(geometry)
	, m_material(material)
	, m_target(target)
	, m_state(state)
{
}

ScreenSpaceRenderPass::~ScreenSpaceRenderPass()
{}

const std::string& ScreenSpaceRenderPass::name() const
{
	return m_name;
}

IRenderTargetSPtr ScreenSpaceRenderPass::target() const
{
	return m_target;
}

bool ScreenSpaceRenderPass::isEnabled() const
{
	return m_enabled;
}

void ScreenSpaceRenderPass::setEnabled(bool flag)
{
	m_enabled = flag;
}

void ScreenSpaceRenderPass::render(Renderer& renderer) const
{
	renderer.setTarget(m_target);

	renderer.applyState(m_state);

	renderer.render(m_fullscreenTriangle, m_material);
}

void ScreenSpaceRenderPass::update(double /*deltaTime*/)
{
}
