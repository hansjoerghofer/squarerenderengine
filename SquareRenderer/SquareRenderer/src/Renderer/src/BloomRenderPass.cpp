#include "Renderer/BloomRenderPass.h"
#include "API/GraphicsAPI.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/ResourceManager.h"
#include "Scene/IGeometry.h"
#include "Texture/Texture2D.h"
#include "Texture/TextureDefines.h"

BloomRenderPass::BloomRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib)
	: BaseRenderPass("Bloom", resources, matlib)
{
}

BloomRenderPass::~BloomRenderPass()
{
}

void BloomRenderPass::setup(IRenderTargetSPtr target, Texture2DSPtr screenBuffer)
{
	BaseRenderPass::setup(target);

	m_sourceBuffer = screenBuffer;

	m_highIntensityBuffer = m_resources->createSimpleColorTarget(
		m_target->width(), 
		m_target->height(), 
		TextureFormat::RGBAHalf,
		{ TextureFilter::Linear, TextureWrap::Mirror, true, glm::vec4(0) });

	m_pingRT = m_resources->createSimpleColorTarget(m_target, m_scale);
	m_pongRT = m_resources->createSimpleColorTarget(m_pingRT);

	m_brightpassFilter = m_matlib->instanciate("Util.HighpassFilter", "Highpass");
	m_brightpassFilter->setUniform("image", m_sourceBuffer);
	m_brightpassFilter->setUniform("threshold", m_brightnessThreshold);

	m_downsampling = m_matlib->instanciate("PP.Blit", "Downsampling");
	m_downsampling->setUniform("image", m_highIntensityBuffer->colorTarget());
	m_downsampling->setUniform("level", std::ceil(std::log2( 1 / m_scale )));

	m_verticalBlur = m_matlib->instanciate("Util.VerticalBlur");
	m_verticalBlur->setUniform("image", m_pongRT->colorTarget());
	m_verticalBlur->setUniform("dim", m_pongRT->dimensions());

	m_horizontalBlur = m_matlib->instanciate("Util.HorizontalBlur");
	m_horizontalBlur->setUniform("image", m_pingRT->colorTarget());
	m_horizontalBlur->setUniform("dim", m_pingRT->dimensions());

	m_blendAdd = m_matlib->instanciate("PP.Blit", "BloomBlend");
	m_blendAdd->setUniform("image", m_pongRT->colorTarget());
}

void BloomRenderPass::renderInternal(Renderer& renderer) const
{
	// extract high luminance values
	blit(renderer, m_highIntensityBuffer, m_brightpassFilter);

	renderer.regenerateMipmaps(m_highIntensityBuffer->colorTarget());

	// downsample
	blit(renderer, m_pongRT, m_downsampling);

	// blur
	for (int i = 0; i < m_numBlurIterations; ++i)
	{
		blit(renderer, m_pingRT, m_verticalBlur);
		blit(renderer, m_pongRT, m_horizontalBlur);
	}

	// blend with output
	blit(renderer, m_target, m_blendAdd, RendererState::Add());
}

void BloomRenderPass::updateInternal(double /*deltaTime*/)
{
	m_brightpassFilter->setUniform("threshold", m_brightnessThreshold);
	m_brightpassFilter->setUniform("intensity", m_intensity);
}

Texture2DSPtr BloomRenderPass::blurBuffer() const
{
	return m_pongRT->colorTargetAs<Texture2D>();
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

float BloomRenderPass::intensity() const
{
	return m_intensity;
}

void BloomRenderPass::setIntensity(float value)
{
	m_intensity = value;
}
