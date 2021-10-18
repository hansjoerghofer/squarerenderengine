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
	m_pingRT = createColorBuffer(
			static_cast<int>(m_target->width() * m_scale),
			static_cast<int>(m_target->height() * m_scale));

	m_pongRT = createColorBuffer(m_pingRT->width(), m_pingRT->height());

	glm::vec4 dim = glm::vec4();
	dim.x = m_pingRT->width();
	dim.y = m_pingRT->height();
	dim.z = 1.f / m_pingRT->width();
	dim.w = 1.f / m_pingRT->height();

	m_brightpassFilter = m_matlib->instanciate("Util.HighpassFilter", "Highpass");
	m_brightpassFilter->setUniform("image", m_pingRT->colorTargets()[0]);
	m_brightpassFilter->setUniform("threshold", m_brightnessThreshold);
	m_highpassPass = std::make_unique<ScreenSpaceRenderPass>("Highpass", m_fullscreenTriangle, m_brightpassFilter, m_pongRT);

	MaterialSPtr matV = m_matlib->instanciate("Util.VerticalBlur");
	matV->setUniform("image", m_pongRT->colorTargets()[0]);
	matV->setUniform("dim", dim);
	m_verticalBlurIt = std::make_unique<ScreenSpaceRenderPass>("VerticalBlur_It", m_fullscreenTriangle, matV, m_pingRT);

	MaterialSPtr matH = m_matlib->instanciate("Util.HorizontalBlur");
	matH->setUniform("image", m_pingRT->colorTargets()[0]);
	matH->setUniform("dim", dim);
	m_horizontalBlurIt = std::make_unique<ScreenSpaceRenderPass>("HorizontalBlur_It", m_fullscreenTriangle, matH, m_pongRT);

	MaterialSPtr matBlit = m_matlib->instanciate("PP.Blit", "BloomBlend");
	//matBlit->setUniform("screenTexture", m_upsampledRT->colorTargets()[0]);
	matBlit->setUniform("screenTexture", m_pongRT->colorTargets()[0]);
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

	// blend with output
	m_blendPass->render(renderer);
}

void BloomRenderPass::update(double /*deltaTime*/)
{
	m_brightpassFilter->setUniform("threshold", m_brightnessThreshold);
}

ITextureSPtr BloomRenderPass::blurBuffer() const
{
	return m_pongRT->colorTargets()[0];
}

int BloomRenderPass::iterations() const
{
	return m_numBlurIterations;
}

void BloomRenderPass::setIterations(int value)
{
	m_numBlurIterations = value;
}

float BloomRenderPass::threshold() const
{
	return m_brightnessThreshold;
}

void BloomRenderPass::setThreshold(float value)
{
	m_brightnessThreshold = value;
}

RenderTargetSPtr BloomRenderPass::createColorBuffer(int width, int height)
{
	TextureSampler sampler;
	sampler.mipmapping = false;
	sampler.wrap = TextureWrap::Mirror;

	RenderTargetSPtr rt = std::make_shared<RenderTarget>(
		std::make_shared<Texture2D>(width, height,
			TextureFormat::RGBAHalf, sampler));

	if (m_api->allocate(rt))
	{
		return rt;
	}

	return nullptr;
}

const std::string& BloomRenderPass::name() const
{
	return m_name;
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
