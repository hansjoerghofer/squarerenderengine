#include "Renderer/RenderEngine.h"
#include "API/GraphicsAPI.h"
#include "Common/Logger.h"
#include "Material/MaterialLibrary.h"
#include "Renderer/BloomRenderPass.h"
#include "Renderer/Camera.h"
#include "Renderer/DepthBuffer.h"
#include "Renderer/GeometryRenderPass.h"
#include "Renderer/GizmoHelper.h"
#include "Renderer/Primitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/ResourceManager.h"
#include "Renderer/ShadowMappingRenderPass.h"
#include "Renderer/SSAORenderPass.h"
#include "Renderer/TonemappingRenderPass.h"
#include "Scene/DirectionalLight.h"
#include "Scene/MeshBuilder.h"
#include "Scene/PointLight.h"
#include "Scene/PrimitiveSet.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Texture/Cubemap.h"
#include "Texture/Texture2D.h"
#include "UniformBlockDataStructs.h"
#include <set>

static const glm::mat4 CUBE_P = glm::perspective(glm::radians(90.f), 1.f, .1f, 10.f);
static const glm::mat4 CUBE_FACE_VP[] = {
	CUBE_P * glm::lookAt(glm::vec3(0,0,0), glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
	CUBE_P * glm::lookAt(glm::vec3(0,0,0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
	CUBE_P * glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
	CUBE_P * glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
	CUBE_P * glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
	CUBE_P * glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 0,-1), glm::vec3(0,-1, 0))
};

RenderEngine::RenderEngine(GraphicsAPISPtr api, MaterialLibrarySPtr matlib)
	: m_api(api)
	, m_matlib(matlib)
	, m_renderer(new Renderer())
	, m_resources(new ResourceManager(api))
	, m_cameraUniformBlock(new UniformBlockData<CameraUniformBlock>(0))
	, m_lightsUniformBlock(new UniformBlockData<LightsUniformBlock>(1))
{
	api->allocate(m_cameraUniformBlock);
	api->allocate(m_lightsUniformBlock);

	for (auto& [name, program] : m_matlib->programs())
	{
		const auto result = api->compile(program);
		if (!result)
		{
			Logger::Error("Error while linking shader program: '%s'\n%s",
				program->name().c_str(), result.message.c_str());

			continue;
		}

		// bind uniform blocks to all programs who need it
		program->bindUniformBlock("CameraUBO", m_cameraUniformBlock->bindingPoint());
		program->bindUniformBlock("LightsUBO", m_lightsUniformBlock->bindingPoint());
	}
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::setScene(SceneSPtr scene)
{
	m_scene = scene;


	if (m_scene->sky())
	{
		m_ibl = generateIBL(std::static_pointer_cast<Cubemap>(m_scene->sky()));
	}

	rebuildCommandList();
}

void RenderEngine::setMainCamera(CameraSPtr camera)
{
	m_mainCamera = camera;

	rebuildCommandList();
}

void RenderEngine::setRenderTarget(IRenderTargetSPtr target)
{
	m_outputTarget = target;

	rebuildCommandList();
}

void RenderEngine::render()
{
	for (const auto& pass : m_renderPassList)
	{
		if (pass->isEnabled())
		{
			pass->render(*m_renderer);
		}
	}
}

void RenderEngine::update(double deltaTime)
{
	if (!m_outputTarget || !m_gBuffer || !m_mainCamera) return;

	if(m_gBuffer->width() != static_cast<int>(m_outputTarget->width() * m_scale) ||
	   m_gBuffer->height() != static_cast<int>(m_outputTarget->height() * m_scale))
	{
		m_mainCamera->updateResolution(
			m_outputTarget->width(), 
			m_outputTarget->height());

		m_gBuffer.reset();
		rebuildCommandList();
	}

	for (const auto& pass : m_renderPassList)
	{
		pass->update(deltaTime);
	}

	updateLightsUniformData();
	updateCameraUniformData(m_mainCamera);
}

void RenderEngine::rebuildCommandList()
{
	m_renderPassList.clear();

	if (!m_mainCamera || !m_outputTarget)
	{
		return;
	}

	if (!m_gBuffer)
	{
		Texture2DSPtr colorBuffer = std::make_shared<Texture2D>(
			static_cast<int>(m_outputTarget->width() * m_scale), 
			static_cast<int>(m_outputTarget->height() * m_scale),
			TextureFormat::RGBAFloat);

		Texture2DSPtr normalsBuffer = std::make_shared<Texture2D>(
			colorBuffer->width(),
			colorBuffer->height(),
			TextureFormat::RGBAFloat);

		std::vector<ITextureSPtr> buffers = { colorBuffer, normalsBuffer };
		m_gBuffer = std::make_shared<RenderTarget>(std::move(buffers), DepthBufferFormat::Depth24Stencil8);

		if (!m_api->allocate(m_gBuffer))
		{
			Logger::Error("Invalid Framebuffer. Abort render command list creation.");
			return;
		}
	}

	// scene
	if (m_scene)
	{
		{
			m_shadowMappingPass = std::make_shared<ShadowMappingRenderPass>(m_resources, m_matlib);
			m_shadowMappingPass->setup(m_scene);
			m_renderPassList.push_back(m_shadowMappingPass);
		}

		GeometryRenderPass::Data cmd;
		cmd.name = "Opaque Scene";
		cmd.target = m_gBuffer;
		cmd.state.clearColor = true;
		//cmd.state.enableWireframe = true;
		cmd.state.color = glm::vec4(.0f, .0f, .0f, 1);
		cmd.state.drawBuffers = { DrawBuffer::Attachment0, DrawBuffer::Attachment1 };

		cmd.drawables.reserve(m_scene->nodeNum());

		std::set<ShaderProgramSPtr> usedPrograms;

		auto t = m_scene->traverser();
		while (t.hasNext())
		{
			IDrawableSPtr drawable = t.next();
			if (drawable->geometry())
			{
				cmd.drawables.push_back(drawable);

				usedPrograms.insert(drawable->material()->program());
			}
		}

		for (ShaderProgramSPtr program : usedPrograms)
		{
			// set IBL
			if (m_ibl.brdf && m_ibl.specular && m_ibl.diffuse)
			{
				program->setUniformDefault("brdfLUT", m_ibl.brdf);
				program->setUniformDefault("prefilterMap", m_ibl.specular);
				program->setUniformDefault("irradianceMap", m_ibl.diffuse);
			}
		}

		// opaque scene rendering
		m_renderPassList.emplace_back(
			new GeometryRenderPass(m_resources, m_matlib, cmd));

		// skybox rendering
		MaterialSPtr skyboxMat = m_matlib->instanciate("Util.Skybox");
		if (skyboxMat && m_scene->sky())
		{
			skyboxMat->setUniform("skybox", m_scene->sky());

			GeometryRenderPass::Data cmd2;
			cmd2.name = "Skybox";
			cmd2.target = m_gBuffer;
			cmd2.state.clearColor = false;
			cmd2.state.clearDepth = false;
			cmd2.state.clearStencil = false;
			cmd2.state.writeDepth = false;
			cmd2.state.cullingMode = Culling::None;
			cmd2.state.depthTestMode = DepthTest::LessEqual;

			IGeometrySPtr geometry = MeshBuilder::skybox();
			if (m_api->allocate(geometry))
			{
				cmd2.drawables.emplace_back(new Primitive(geometry, skyboxMat));

				m_renderPassList.emplace_back(
					new GeometryRenderPass(m_resources, m_matlib, cmd2));
			}			
		}
	}

	setupPostProcessing();

	if (m_gizmos)
	{
		// TODO use gBuffer depth as depth buffer for
		// occluded bounding boxes!

		GeometryRenderPass::Data cmd;
		cmd.name = "Gizmos";
		cmd.target = m_outputTarget;
		cmd.state.clearColor = false;
		cmd.state.clearDepth = false;
		cmd.state.writeDepth = false;
		//cmd.state.depthTestMode = DepthTest::LessEqual;
		cmd.drawables.push_back(m_gizmos);

		m_renderPassList.emplace_back(
			new GeometryRenderPass(m_resources, m_matlib, cmd));
	}
}

void RenderEngine::setupGizmos(const std::string& programName)
{
	MaterialSPtr material = m_matlib->instanciate(programName);
	if (!material)
	{
		Logger::Warning("Could not instanciate gizmo material '%s'", 
			programName.c_str());
		return;
	}

	std::vector<LineSegment> lines;

	const auto axisGizmo = GizmoHelper::createCoordinateAxis();
	lines.insert(lines.end(), axisGizmo.begin(), axisGizmo.end());

	if (m_scene)
	{
		// light sources
		for (ILightsourceSPtr light : m_scene->lights())
		{
			const auto lightGizmo = GizmoHelper::createLightsource(light);
			lines.insert(lines.end(), lightGizmo.begin(), lightGizmo.end());
		}

		//bounding boxes
		/*auto t = m_scene->traverser();
		while (t.hasNext())
		{
			SceneNodeSPtr node = t.next();
			
			if (node->bounds().empty()) continue;

			const BoundingBox worldBounds = node->worldTransform() * node->bounds();

			const auto aabbGizmo = GizmoHelper::createBoundingBox(worldBounds);
			lines.insert(lines.end(), aabbGizmo.begin(), aabbGizmo.end());
		}

		const auto aabbGizmo = GizmoHelper::createBoundingBox(m_scene->sceneBounds());
		lines.insert(lines.end(), aabbGizmo.begin(), aabbGizmo.end());*/
	}

	std::vector<Vertex> vertices = GizmoHelper::linesToPrimitives(lines);

	IGeometrySPtr gizmoGeometry = std::make_shared<PrimitiveSet>(
		PrimitiveType::Lines, std::move(vertices), Vertex::DATA_NORMAL);

	if (!m_api->allocate(gizmoGeometry))
	{
		Logger::Warning("Error while allocating gizmo geometry buffers.");
		return;
	}

	m_gizmos = std::make_shared<Primitive>(gizmoGeometry, material);

	rebuildCommandList();
}

void RenderEngine::setupPostProcessing()
{
	if (!m_gBuffer)
	{
		return;
	}

	{
		BloomRenderPassSPtr bloomPass = std::make_shared<BloomRenderPass>(m_resources, m_matlib);
		bloomPass->setup(m_gBuffer, m_gBuffer->colorTargetAs<Texture2D>());
		m_renderPassList.push_back(bloomPass);
	}

	// TODO cannot render to both PP passes! why? draw/read conflict?
	/*{
		SSAORenderPassSPtr ssaoPass = std::make_shared<SSAORenderPass>(m_resources, m_matlib);
		ssaoPass->setup(m_gBuffer, m_gBuffer->colorTargetAs<Texture2D>(0), m_gBuffer->colorTargetAs<Texture2D>(1));
		m_renderPassList.push_back(ssaoPass);
	}*/

	{
		TonemappingRenderPassSPtr tonemappingPass = std::make_shared<TonemappingRenderPass>(m_resources, m_matlib);
		tonemappingPass->setup(m_outputTarget, m_gBuffer);
		m_renderPassList.push_back(tonemappingPass);
	}
}

RenderTargetSPtr RenderEngine::screenSpaceTarget(float scale)
{
	TextureSampler sampler;
	sampler.mipmapping = false;
	RenderTargetSPtr rt = std::make_unique<RenderTarget>(
		std::make_shared<Texture2D>(
			static_cast<int>(m_outputTarget->width() * scale),
			static_cast<int>(m_outputTarget->height() * scale),
			TextureFormat::RGBAFloat, sampler));
	
	if (m_api->allocate(rt))
	{
		return rt;
	}
	else
	{
		Logger::Error("Could not allocate screen render target");
		return nullptr;
	}
}

ITextureSPtr RenderEngine::getLastColorTarget(int slotIndex) const
{
	RenderTargetSPtr previousTarget = std::static_pointer_cast<RenderTarget>(
		m_renderPassList.back()->target());

	return previousTarget->colorTargets()[slotIndex];
}

IRenderTargetSPtr RenderEngine::renderTarget() const
{
	return m_outputTarget;
}

void RenderEngine::setRenderingScale(double scale)
{
	m_scale = scale;
}

const std::list<IRenderPassSPtr>& RenderEngine::renderPasses() const
{
	return m_renderPassList;
}

void RenderEngine::updateCameraUniformData(CameraSPtr camera)
{
	if (!camera)
	{
		return;
	}

	CameraUniformBlock data = CameraUniformBlock();
	data.P = camera->projectionMatrix();
	data.V = camera->viewMatrix();

	data.invP = glm::inverse(data.P);
	data.invV = glm::inverse(data.V);

	data.VP = data.P * data.V;

	data.dim.x = static_cast<float>(camera->width());
	data.dim.y = static_cast<float>(camera->height());
	data.dim.z = 1.f / data.dim.x;
	data.dim.w = 1.f / data.dim.y;

	data.clip.x = camera->near();
	data.clip.y = camera->far();
	data.clip.z = 1.f / data.clip.x;
	data.clip.w = 1.f / data.clip.y;

	m_cameraUniformBlock->update(data);
}

void RenderEngine::updateLightsUniformData()
{
	LightsUniformBlock data = LightsUniformBlock();
	data.ambientColor = glm::vec4(.1, .1, .1, 1);
	data.numLights = 0;

	for (ILightsourceSPtr light : m_scene->lights())
	{
		if (data.numLights == MAX_LIGHT_COUNT)
		{
			break;
		}

		glm::vec4 vec;
		switch (light->type())
		{
		case LightsourceType::Directional:
			vec = glm::vec4(std::static_pointer_cast<DirectionalLight>(light)->direction(), 0);
			break;
		case LightsourceType::Point:
			vec = glm::vec4(std::static_pointer_cast<PointLight>(light)->position(), 1);
			break;
		default:
			continue;
		}

		data.lightsPosWS[data.numLights] = vec;
		data.lightsColor[data.numLights] = glm::vec4(light->color(), light->intensity());
		
		ShadowData shadowData;
		auto found = m_shadowMappingPass->shadowData().find(light);
		if (found != m_shadowMappingPass->shadowData().end())
		{
			shadowData = found->second;
		}
		data.lightsMatrix[data.numLights] = shadowData.lightMatrice;
		data.shadowMapIndex[data.numLights] = glm::vec4(shadowData.index, 0, 0, 0);
		
		data.numLights++;
	}

	m_lightsUniformBlock->update(data);
}

void RenderEngine::projectEquirectangularToCubemap(Texture2DSPtr source, CubemapSPtr target)
{
	if (source->width() != source->height() * 2)
	{
		throw std::invalid_argument("Invalid texture dimension for LongLat map.");
	}

	RenderTargetSPtr rt = std::make_shared<RenderTarget>(target);
	if (!m_api->allocate(rt))
	{
		return;
	}

	IGeometrySPtr cubeGeom = MeshBuilder::cube();
	if (!m_api->allocate(cubeGeom))
	{
		Logger::Warning("Error while allocating cube geometry.");
		return;
	}

	MaterialSPtr projectionMat = m_matlib->instanciate("Util.ProjectEqr2Cube");
	if (!projectionMat)
	{
		return;
	}

	const int rotXDeg = 0;
	projectionMat->setUniform("horizontalRotation", (rotXDeg % 360) / 360.f);
	projectionMat->setUniform("equirectangularMap", source);

	for (int i = 0; i < 6; ++i)
	{
		//projectionMat->setUniform("VP[" + std::to_string(i) + "]", CUBE_FACE_VP[i]);
		projectionMat->setUniform("VP", CUBE_FACE_VP[i], i);
	}

	GeometryRenderPass::Data cmd;
	cmd.name = "EquirectangularToCubemap";
	cmd.target = rt;
	cmd.drawables.emplace_back(new Primitive(cubeGeom, projectionMat));

	cmd.state.clearColor = true;
	cmd.state.color = glm::vec4(0, 0, 0, 1);
	cmd.state.cullingMode = Culling::Front;
	cmd.state.depthTestMode = DepthTest::Equal;
	cmd.state.writeDepth = false;

	const auto pass = GeometryRenderPass(m_resources, m_matlib, cmd);
	pass.render(*m_renderer);

	m_renderer->regenerateMipmaps(target);
}

IBLData RenderEngine::generateIBL(CubemapSPtr hdri)
{
	const int diffuseRes = 32;
	const int specularRes = 128;
	const int brdfRes = 512;

	IBLData ibl;

	TextureSampler sampler;
	sampler.wrap = TextureWrap::ClampToEdge;
	sampler.mipmapping = true;
	ibl.specular = std::make_shared<Cubemap>(specularRes, TextureFormat::RGBHalf, sampler);

	sampler.mipmapping = false;
	ibl.diffuse = std::make_shared<Cubemap>(diffuseRes, TextureFormat::RGBHalf, sampler);
	ibl.brdf = std::make_shared<Texture2D>(brdfRes, brdfRes, TextureFormat::RGHalf, sampler);

	hdriToDiffuseIrradiance(hdri, ibl.diffuse);
	hdriToSpecularIrradiance(hdri, ibl.specular);
	generateIntegratedBRDF(ibl.brdf);

	return ibl;
}

void RenderEngine::hdriToDiffuseIrradiance(CubemapSPtr hdri, CubemapSPtr diffIrradiance)
{
	RenderTargetSPtr rt = std::make_shared<RenderTarget>(diffIrradiance);
	if (!m_api->allocate(rt))
	{
		return;
	}

	IGeometrySPtr cubeGeom = MeshBuilder::cube();
	if (!m_api->allocate(cubeGeom))
	{
		Logger::Warning("Error while allocating cube geometry.");
		return;
	}

	MaterialSPtr mat = m_matlib->instanciate("Util.IBLDiffuse");
	if (!mat)
	{
		return;
	}

	mat->setUniform("hdri", hdri);

	for (int i = 0; i < 6; ++i)
	{
		mat->setUniform("VP", CUBE_FACE_VP[i], i);
	}

	GeometryRenderPass::Data cmd;
	cmd.name = "HDRI2DiffuseIrradiance";
	cmd.target = rt;
	cmd.drawables.emplace_back(new Primitive(cubeGeom, mat));

	cmd.state.cullingMode = Culling::Front;
	cmd.state.depthTestMode = DepthTest::None;
	cmd.state.writeDepth = false;
	cmd.state.clearDepth = false;
	cmd.state.seamlessCubemapFiltering = true;

	const auto pass = GeometryRenderPass(m_resources, m_matlib, cmd);
	pass.render(*m_renderer);
}

void RenderEngine::hdriToSpecularIrradiance(CubemapSPtr hdri, CubemapSPtr specIrradiance)
{
	IGeometrySPtr cubeGeom = MeshBuilder::cube();
	if (!m_api->allocate(cubeGeom))
	{
		Logger::Warning("Error while allocating cube geometry.");
		return;
	}

	MaterialSPtr mat = m_matlib->instanciate("Util.IBLSpecular");
	if (!mat)
	{
		return;
	}

	mat->setUniform("hdri", hdri);
	mat->setUniform("resolution", static_cast<float>(hdri->width()));

	for (int i = 0; i < 6; ++i)
	{
		mat->setUniform("VP", CUBE_FACE_VP[i], i);
	}

	RendererState state;
	state.cullingMode = Culling::Front;
	state.depthTestMode = DepthTest::None;
	state.writeDepth = false;
	state.clearDepth = false;
	state.seamlessCubemapFiltering = true;
	m_renderer->applyState(state);

	// --- render ----
	const unsigned int resolution = specIrradiance->width();
	unsigned int maxMipLevels = static_cast<unsigned int>(std::ceil(std::log2(specIrradiance->width())));
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{	
		float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
		mat->setUniform("roughness", roughness);

		RenderTargetSPtr rt = std::make_shared<RenderTarget>(specIrradiance, nullptr, mip);
		if (!m_api->allocate(rt))
		{
			return;
		}
		
		m_renderer->setTarget(rt);
		m_renderer->render(cubeGeom, mat);
	}
}

void RenderEngine::generateIntegratedBRDF(Texture2DSPtr integratedBRDF)
{
	RenderTargetSPtr rt = std::make_shared<RenderTarget>(integratedBRDF);
	if (!m_api->allocate(rt))
	{
		return;
	}

	MaterialSPtr brdfMat = m_matlib->instanciate("Util.IntegratedBRDF");
	if (!brdfMat)
	{
		return;
	}

	m_renderer->setTarget(rt);
	m_renderer->applyState(RendererState::Blit());
	m_renderer->render(m_resources->fullscreenGeometry(), brdfMat);
}

void RenderEngine::packTextures(
	Texture2DSPtr target, 
	Texture2DSPtr sourceRed, 
	Texture2DSPtr sourceGreen, 
	Texture2DSPtr sourceBlue, 
	Texture2DSPtr sourceAlpha)
{
	RenderTargetSPtr rt = std::make_shared<RenderTarget>(target);
	if (!m_api->allocate(rt))
	{
		return;
	}

	MaterialSPtr mat = m_matlib->instanciate("Util.CombineChannels");
	if (!mat)
	{
		return;
	}

	mat->setUniform("redChannel", sourceRed);
	mat->setUniform("greenChannel", sourceGreen);
	mat->setUniform("blueChannel", sourceBlue);
	mat->setUniform("alphaChannel", sourceAlpha);

	m_renderer->setTarget(rt);
	m_renderer->applyState(RendererState::Blit());
	m_renderer->render(m_resources->fullscreenGeometry(), mat);
	m_renderer->regenerateMipmaps(target);
}
