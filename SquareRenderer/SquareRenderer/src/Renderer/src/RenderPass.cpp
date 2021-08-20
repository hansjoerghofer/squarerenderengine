#include "Renderer/RenderPass.h"

#include "Renderer/IDrawable.h"
#include "Renderer/IRenderTarget.h"
#include "Material/Material.h"

RenderPass::RenderPass(const RenderCommand& command)
	: m_command(command)
{
}

RenderPass::RenderPass(RenderCommand&& command)
	: m_command(command)
{
}

RenderPass::~RenderPass()
{
}

std::string RenderPass::name() const
{
	return m_command.name;
}

bool RenderPass::isEnabled() const
{
	return m_command.enabled;
}

void RenderPass::setEnabled(bool flag)
{
	m_command.enabled = flag;
}

const RendererState& RenderPass::rendererState() const
{
	return m_command.state;
}

IRenderTargetSPtr RenderPass::target() const
{
	return m_command.target;
}

MaterialSPtr RenderPass::material() const
{
	return m_command.overrideMaterial;
}

const std::vector<IDrawableSPtr>& RenderPass::drawables() const
{
	return m_command.drawables;
}

void RenderPass::update(double /*deltaTime*/)
{
}
