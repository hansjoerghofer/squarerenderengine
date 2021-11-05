#include "Common/Logger.h"
#include "API/GraphicsAPI.h"
#include "Renderer/BaseRenderPass.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"
#include "Renderer/ResourceManager.h"
#include "Material/MaterialLibrary.h"
#include "Material/Material.h"
#include "Texture/Texture2D.h"
#include "Texture/TextureDefines.h"

BaseRenderPass::BaseRenderPass(const std::string& name, ResourceManagerSPtr resources, MaterialLibrarySPtr matlib)
	: m_name(name)
	, m_resources(resources)
	, m_matlib(matlib)
	, m_gpuTimer(new GPUTimer())
{
}

BaseRenderPass::~BaseRenderPass()
{
}

const std::string& BaseRenderPass::name() const
{
	return m_name;
}

void BaseRenderPass::render(Renderer& renderer) const
{
	if (!m_enableRendering) return;

	Logger::Debug("-- Start renderpass %s", m_name.c_str());
	GraphicsAPIBeginScopedDebugGroup("PASS: " + name());

	renderer.resetPrimitiveCounter();

	m_timer.reset();
	m_gpuTimer->begin();
	
	renderInternal(renderer);

	m_gpuTimer->end();
	m_renderStatistics.cpuTimeMs += m_timer.elapsedMs();
	m_renderStatistics.rendererdPrimitives = renderer.primitiveCounter();
}

void BaseRenderPass::update(double deltaTime)
{
	if (!m_enableUpdate) return;

	m_timer.reset();

	updateInternal(deltaTime);

	m_renderStatistics.cpuTimeMs = m_timer.elapsedMs();
}

void BaseRenderPass::setup(IRenderTargetSPtr target)
{
	m_target = target;
}

bool BaseRenderPass::isEnabled() const
{
	return m_enabled;
}

void BaseRenderPass::setEnabled(bool flag)
{
	m_enabled = flag;
	m_enableRendering = m_enabled;
	m_enableUpdate = m_enabled;
}

IRenderTargetSPtr BaseRenderPass::target() const
{
	return m_target;
}

RenderStatisticsData BaseRenderPass::renderStatistics() const
{
	// defer fetching after rendering
	m_renderStatistics.gpuTimeMs = m_gpuTimer->fetchElapsedMs();

	return m_renderStatistics;
}

void BaseRenderPass::updateInternal(double /*deltaTime*/)
{
}

void BaseRenderPass::blit(Renderer& renderer,
	IRenderTargetSPtr target, 
	MaterialSPtr material, 
	const RendererState& state /*= RendererState::Blit()*/) const
{
	renderer.setTarget(target);

	renderer.applyState(state);

	renderer.render(m_resources->fullscreenGeometry(), material);
}
