#include "Renderer/TonemappingRenderPass.h"
#include "Renderer/IRenderTarget.h"
#include "Renderer/RenderTarget.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"

TonemappingRenderPass::TonemappingRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib)
	: BaseRenderPass("Tonemapping", resources, matlib)
{
}

TonemappingRenderPass::~TonemappingRenderPass()
{
}

void TonemappingRenderPass::setup(IRenderTargetSPtr target, RenderTargetSPtr input)
{
	BaseRenderPass::setup(target);

	m_input = input;

	m_tonemapping = m_matlib->instanciate("PP.Tonemapping");
	m_tonemapping->setUniform("screenTexture", m_input->colorTarget());

	m_passthrough = m_matlib->instanciate("PP.Blit");
	m_passthrough->setUniform("image", m_input->colorTarget());
}

void TonemappingRenderPass::setEnabled(bool flag)
{
	BaseRenderPass::setEnabled(flag);
	m_enableRendering = true;
}

void TonemappingRenderPass::renderInternal(Renderer& renderer) const
{
	//blit(renderer, m_target, m_enabled ? m_tonemapping : m_passthrough);
	
	if (isEnabled())
	{
		blit(renderer, m_target, m_tonemapping);
	}
	else
	{
		blit(renderer, m_target, m_passthrough);
	}
}
