#include "API/GraphicsAPI.h"
#include "Renderer/BloomRenderPass.h"
#include "Material/MaterialLibrary.h"
#include "Material/Material.h"
#include "Scene/IGeometry.h"
#include "Renderer/RenderTarget.h"
#include "Texture/Texture2D.h"
#include "Texture/TextureDefines.h"
#include "Renderer/Renderer.h"
#include "Renderer/ScreenSpaceRenderPass.h"

BloomRenderPass::BloomRenderPass(GraphicsAPISPtr api, MaterialLibrarySPtr matlib, IGeometrySPtr fullscreenTriangle, IRenderTargetSPtr target)
	: m_api(api)
	, m_matlib(matlib)
	, m_fullscreenTriangle(fullscreenTriangle)
	, m_target(target)
{
	setup();
}

BloomRenderPass::~BloomRenderPass()
{
}

void BloomRenderPass::setup()
{
	TextureSampler sampler;
	sampler.mipmapping = false;
	
	m_upsampledRT = std::make_unique<RenderTarget>(
		std::make_shared<Texture2D>(
			static_cast<int>(m_target->width()),
			static_cast<int>(m_target->height()),
			TextureFormat::RGBAHalf, sampler));
	m_api->allocate(m_upsampledRT);

	m_pingRT = std::make_unique<RenderTarget>(
		std::make_shared<Texture2D>(
			static_cast<int>(m_target->width() * m_scale),
			static_cast<int>(m_target->height() * m_scale),
			TextureFormat::RGBAHalf, sampler));
	m_api->allocate(m_pingRT);

	m_pongRT = std::make_unique<RenderTarget>(
		std::make_shared<Texture2D>(
			static_cast<int>(m_pingRT->width()),
			static_cast<int>(m_pingRT->height()),
			TextureFormat::RGBAHalf, sampler));
	m_api->allocate(m_pongRT);

	glm::vec4 dim = glm::vec4();
	dim.x = m_pingRT->width();
	dim.y = m_pingRT->height();
	dim.z = 1.f / m_pingRT->width();
	dim.w = 1.f / m_pingRT->height();

	MaterialSPtr mat = m_matlib->instanciate("Util.HighpassFilter", "Highpass");
	mat->setUniform("image", m_pingRT->colorTargets()[0]);
	mat->setUniform("threshold", m_brightnessThreshold);
	m_highpassPass = std::make_unique<ScreenSpaceRenderPass>("Highpass", m_fullscreenTriangle, mat, m_pongRT);

	MaterialSPtr matV = m_matlib->instanciate("Util.VerticalBlur");
	matV->setUniform("image", m_pongRT->colorTargets()[0]);
	matV->setUniform("dim", dim);
	m_verticalBlurIt = std::make_unique<ScreenSpaceRenderPass>("VerticalBlur_It", m_fullscreenTriangle, matV, m_pingRT);

	MaterialSPtr matH = m_matlib->instanciate("Util.HorizontalBlur");
	matH->setUniform("image", m_pingRT->colorTargets()[0]);
	matH->setUniform("dim", dim);
	m_horizontalBlurIt = std::make_unique<ScreenSpaceRenderPass>("HorizontalBlur_It", m_fullscreenTriangle, matH, m_pongRT);

	MaterialSPtr matBlit = m_matlib->instanciate("PP.Blit");
	matBlit->setUniform("screenTexture", m_upsampledRT->colorTargets()[0]);
	RendererState blendState = RendererState::Blit();
	blendState.clearColor = false;
	blendState.blendSrc = BlendFactor::One;
	blendState.blendDst = BlendFactor::One;
	m_blendPass = std::make_unique<ScreenSpaceRenderPass>("BlendBloom", m_fullscreenTriangle, matBlit, m_target, blendState);
}

void BloomRenderPass::render(Renderer& renderer) const
{
	// downsample
	renderer.blit(m_target, m_pingRT);

	// extract high luminance values
	m_highpassPass->render(renderer);

	// blur
	for (int i = 0; i < m_numBlurIterations; ++i)
	{
		m_verticalBlurIt->render(renderer);
		m_horizontalBlurIt->render(renderer);
	}

	// upsample
	renderer.blit(m_pongRT, m_upsampledRT);

	// blend with output
	m_blendPass->render(renderer);
}

void BloomRenderPass::update(double /*deltaTime*/)
{
}

const std::string& BloomRenderPass::name() const
{
	return "Bloom";
}

IRenderTargetSPtr BloomRenderPass::target() const
{
	return m_target;
}

bool BloomRenderPass::isEnabled() const
{
	return m_enabled;
}

void BloomRenderPass::setEnabled(bool flag)
{
	m_enabled = flag;
}
