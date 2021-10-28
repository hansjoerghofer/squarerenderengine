#include "Renderer/SSAORenderPass.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Renderer/IRenderTarget.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/ResourceManager.h"
#include "Scene/IGeometry.h"
#include "Texture/Texture2D.h"
#include "Texture/TextureDefines.h"

SSAORenderPass::SSAORenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib)
	: BaseRenderPass("SSAO", resources, matlib)
{
	/*m_aoBuffer = createColorBuffer(target->width(), target->height());

	MaterialSPtr matBlit = m_matlib->instanciate("PP.SSAO", "SSAO");
	matBlit->setUniform("screenTexture", m_aoBuffer->colorTargets()[1]);
	RendererState blendState = RendererState::Blit();
	blendState.clearColor = false;
	blendState.blendSrc = BlendFactor::One;
	blendState.blendDst = BlendFactor::One;
	m_aoPass = std::make_unique<ScreenSpaceRenderPass>("BlendBloom", m_fullscreenTriangle, matBlit, m_target, blendState);*/
}

SSAORenderPass::~SSAORenderPass()
{
}

void SSAORenderPass::setup(IRenderTargetSPtr target, Texture2DSPtr /*screenBuffer*/, Texture2DSPtr normalsDepthBuffer)
{
	BaseRenderPass::setup(target);

	m_aoBuffer = m_resources->createSimpleColorTarget(target);

	//m_inputBuffer = m_resources->allocateRenderTarget(std::make_shared<RenderTarget>(normalsDepthBuffer));

	m_aoMaterial = m_matlib->instanciate("PP.SSAO");
	m_aoMaterial->setUniform("normalsDepthTexture", normalsDepthBuffer);

	m_blitMaterial = m_matlib->instanciate("PP.Blit");
	m_blitMaterial->setUniform("image", m_aoBuffer->colorTarget());
}

void SSAORenderPass::renderInternal(Renderer& renderer) const
{
	//renderer.blit(m_inputBuffer, m_target);

	blit(renderer, m_aoBuffer, m_aoMaterial);

	blit(renderer, m_target, m_blitMaterial);
}

void SSAORenderPass::update(double /*deltaTime*/)
{
}