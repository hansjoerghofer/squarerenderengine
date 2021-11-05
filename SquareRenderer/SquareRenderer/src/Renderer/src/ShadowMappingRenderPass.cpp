#include "Renderer/ShadowMappingRenderPass.h"
#include "API/GraphicsAPI.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Renderer/DepthBuffer.h"
#include "Renderer/IDrawable.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/ResourceManager.h"
#include "Scene/BoundingBox.h"
#include "Scene/DirectionalLight.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Texture/Texture2D.h"

#include <set>

constexpr glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0);

ShadowMappingRenderPass::ShadowMappingRenderPass(ResourceManagerSPtr resources, MaterialLibrarySPtr matlib)
	: BaseGeometryRenderPass("Shadow Mapping", resources, matlib)
{
}

ShadowMappingRenderPass::~ShadowMappingRenderPass()
{
}

glm::mat4 calculateLightSpaceMatrix(DirectionalLightSPtr light, const BoundingBox& bounds)
{
	const glm::mat4 lightView = glm::lookAt(
		bounds.center(),
		bounds.center() + light->direction(),
		glm::vec3_up);

	const BoundingBox cameraVolume = lightView * bounds;

	const glm::mat4 lightProj = glm::ortho(
		cameraVolume.min().x, cameraVolume.max().x,
		cameraVolume.min().y, cameraVolume.max().y,
		cameraVolume.min().z, cameraVolume.max().z);

	return lightProj * lightView;
}

void ShadowMappingRenderPass::setup(SceneSPtr scene)
{
	BaseRenderPass::setup(nullptr);

	m_shadowData.clear();
	m_scene = scene;

	m_state = RendererState();
	m_state.clearColor = false;
	m_state.writeColor = false;
	m_state.depthOffset = glm::vec2(9., 1.);

	const int shadowMapWidth = 1024;
	const int shadowMapHeight = 1024;

	constexpr TextureSampler depthMapSampler = {
		TextureFilter::Linear,
		TextureWrap::ClampToBorder,
		false, glm::vec4(1,1,1,1)
	};

	int index = 0;
	for (ILightsourceSPtr light : m_scene->lights())
	{
		if (!light->isShadowCaster()) continue;

		if (light->type() != LightsourceType::Directional) continue;

		// create shadow FBO
		Texture2DSPtr shadowMap = std::make_shared<Texture2D>(shadowMapWidth, shadowMapHeight,
			TextureFormat::DepthFloat, depthMapSampler);

		RenderTargetSPtr shadowMapTarget = std::make_shared<RenderTarget>(
			std::make_shared<DepthTextureWrapper>(shadowMap));

		if (!m_resources->allocateRenderTarget(shadowMapTarget))
		{
			continue;
		}

		DirectionalLightSPtr dirLight = std::static_pointer_cast<DirectionalLight>(light);

		const glm::mat4 worldToLight = calculateLightSpaceMatrix(dirLight, m_scene->sceneBounds());

		MaterialSPtr shadowMat = m_matlib->instanciate("Util.ShadowMapping");
		shadowMat->setUniform("worldToLight", worldToLight);

		ShadowData& sData = m_shadowData[dirLight];
		sData.index = index;
		sData.lightMatrice = BIAS_MATRIX * worldToLight;
		sData.target = shadowMapTarget;
		sData.material = shadowMat;

		++index;
	}

	m_geometry.clear();
	m_geometry.reserve(m_scene->nodeNum());

	std::set<ShaderProgramSPtr> usedPrograms;

	auto t = m_scene->traverser();
	while (t.hasNext())
	{
		IDrawableSPtr drawable = t.next();
		if (drawable->geometry() && drawable->material())
		{
			if (drawable->material()->layer() == Material::Layer::Opaque)
			{
				m_geometry.push_back(drawable);
				usedPrograms.insert(drawable->material()->program());
			}
		}
	}

	for (ShaderProgramSPtr program : usedPrograms)
	{
		//TODO check if shadows are supported?
		// set shadow maps
		for (const auto& [light, shadowData] : m_shadowData)
		{
			program->setUniformDefault("_shadowMapDim", shadowData.target->dimensions());
			program->setUniformDefault("_shadowMaps",
				shadowData.target->depthBufferAs<DepthTextureWrapper>()->texture(), 
				shadowData.index);
		}
	}
}

const std::unordered_map<ILightsourceSPtr, ShadowData>& ShadowMappingRenderPass::shadowData() const
{
	return m_shadowData;
}

void ShadowMappingRenderPass::setDepthOffsetFactor(float factor)
{
	m_state.depthOffset.x = factor;
}

void ShadowMappingRenderPass::setDepthOffsetUnit(float unit)
{
	m_state.depthOffset.y = unit;
}

float ShadowMappingRenderPass::depthOffsetFactor() const
{
	return m_state.depthOffset.x;
}

float ShadowMappingRenderPass::depthOffsetUnit() const
{
	return m_state.depthOffset.y;
}

void ShadowMappingRenderPass::renderInternal(Renderer& renderer) const
{
	for (const auto& [light, shadowData] : m_shadowData)
	{
		GraphicsAPIBeginScopedDebugGroup("Light: #" + std::to_string(shadowData.index));

		renderer.setTarget(shadowData.target);

		renderer.applyState(m_state);

		renderGeometry(renderer, m_geometry, shadowData.material);
	}
}

void ShadowMappingRenderPass::updateInternal(double /*deltaTime*/)
{
	for (auto& [light, shadowData] : m_shadowData)
	{
		if (light->type() == LightsourceType::Directional)
		{
			const glm::mat4 worldToLight = calculateLightSpaceMatrix(
				std::static_pointer_cast<DirectionalLight>(light),
				m_scene->sceneBounds());

			shadowData.lightMatrice = BIAS_MATRIX * worldToLight;
			shadowData.material->setUniform("worldToLight", worldToLight);
		}
	}
}
