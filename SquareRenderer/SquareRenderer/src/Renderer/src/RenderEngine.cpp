#include "Renderer/RenderEngine.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/DepthBuffer.h"
#include "Renderer/RenderPass.h"

#include "Common/Logger.h"
#include "Common/Timer.h"
#include "Common/Math3D.h"

#include "API/GraphicsAPI.h"
#include "API/SharedResource.h"

#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/Mesh.h"
#include "Scene/PrimitiveSet.h"
#include "Scene/SceneNode.h"
#include "Scene/DirectionalLight.h"
#include "Scene/PointLight.h"
#include "Scene/MeshBuilder.h"

#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Material/ShaderProgram.h"

#include "Renderer/Camera.h"
#include "Renderer/Primitive.h"
#include "Renderer/UniformBlockData.h"
#include "Renderer/GizmoHelper.h"

#include "Texture/Texture2D.h"
#include "Texture/Cubemap.h"

#include "UniformBlockDataStructs.h"

#include <set>

RenderEngine::RenderEngine(GraphicsAPISPtr api, MaterialLibrarySPtr matlib)
	: m_api(api)
	, m_matlib(matlib)
	, m_renderer(new Renderer())
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

	if (!m_fullscreenTriangle)
	{
		IGeometrySPtr fullscreenTriangle = MeshBuilder::screenTriangle();
		if (!m_api->allocate(fullscreenTriangle))
		{
			Logger::Warning("Error while allocating fullscreen triangle buffer.");
			return;
		}

		m_fullscreenTriangle = fullscreenTriangle;
	}
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::setScene(SceneSPtr scene)
{
	m_scene = scene;

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
		render(*pass);
	}
}

void RenderEngine::render(const RenderPass& pass)
{
	m_renderer->setTarget(pass.target());

	m_renderer->applyState(pass.rendererState());

	for (const auto& elem : pass.drawables())
	{
		// TODO override logic!
		MaterialSPtr mat = pass.material() ? pass.material() : elem->material();
		if (mat)
		{
			elem->preRender(mat);
			m_renderer->render(elem->geometry(), mat);
			elem->postRender();
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

	updateLightsUniformData();
	updateCameraUniformData(m_mainCamera);

	for (const auto& pass : m_renderPassList)
	{
		pass->update(deltaTime);
	}
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

		m_gBuffer = std::make_shared<RenderTarget>(
			colorBuffer, 
			DepthBufferFormat::Depth24Stencil8);

		if (!m_api->allocate(m_gBuffer))
		{
			Logger::Error("Invalid Framebuffer. Abort render command list creation.");
			return;
		}
	}

	// scene
	if (m_scene)
	{
		setupShadowMapping();

		RenderCommand cmd;
		cmd.name = "Opaque Scene";
		cmd.target = m_gBuffer;
		cmd.state.clearColor = true;
		//cmd.state.enableWireframe = true;
		cmd.state.color = glm::vec4(.0f, .0f, .0f, 1);

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
			// set shadow maps
			for (const auto& [light, shadowData] : m_shadowData)
			{
				const glm::vec4 shadowDim = glm::vec4(
					shadowData.data->width(),
					shadowData.data->height(),
					1.f / shadowData.data->width(),
					1.f / shadowData.data->height()
				);
				program->setUniformDefault("_shadowMapDim", shadowDim);
				// TODO create indexed setter
				program->setUniformDefault(
					"_shadowMaps[" + std::to_string(shadowData.index) + "]",
					shadowData.data);
			}
		}

		// opaque scene rendering
		m_renderPassList.emplace_back(
			new RenderPass(std::move(cmd)));

		// skybox rendering
		MaterialSPtr skyboxMat = m_matlib->instanciate("Util.Skybox");
		if (skyboxMat && m_scene->skybox())
		{
			skyboxMat->setUniform("skybox", m_scene->skybox());

			RenderCommand cmd2;
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
					new RenderPass(std::move(cmd2)));
			}			
		}
	}

	setupPostProcessing();

	if (m_gizmos)
	{
		// TODO use gBuffer depth as depth buffer for
		// occluded bounding boxes!

		RenderCommand cmd;
		cmd.name = "Gizmos";
		cmd.target = m_outputTarget;
		cmd.state.clearColor = false;
		cmd.state.clearDepth = false;
		cmd.state.writeDepth = false;
		//cmd.state.depthTestMode = DepthTest::LessEqual;
		cmd.state.primitive = PrimitiveMode::Lines;
		cmd.drawables.push_back(m_gizmos);

		m_renderPassList.emplace_back(
			new RenderPass(std::move(cmd)));
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
		auto t = m_scene->traverser();
		while (t.hasNext())
		{
			SceneNodeSPtr node = t.next();
			
			if (node->bounds().empty()) continue;

			const BoundingBox worldBounds = node->worldTransform() * node->bounds();

			const auto aabbGizmo = GizmoHelper::createBoundingBox(worldBounds);
			lines.insert(lines.end(), aabbGizmo.begin(), aabbGizmo.end());
		}

		const auto aabbGizmo = GizmoHelper::createBoundingBox(m_scene->sceneBounds());
		lines.insert(lines.end(), aabbGizmo.begin(), aabbGizmo.end());
	}

	std::vector<Vertex> vertices = GizmoHelper::linesToPrimitives(lines);

	IGeometrySPtr gizmoGeometry = std::make_shared<PrimitiveSet>(
		PrimitiveType::Lines, std::move(vertices), false, true);

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

	MaterialSPtr tonemappingMat = m_matlib->instanciate("PP.Tonemapping", "Tonemapping");
	if (tonemappingMat)
	{
		tonemappingMat->setUniform("screenTexture", m_gBuffer->colorTargets()[0]);

		RenderCommand cmd;
		cmd.name = "Tonemapping";
		cmd.target = m_outputTarget;

		//cmd.state.writeDepth = false; // TODO check!
		cmd.state.clearDepth = false;
		cmd.state.depthTestMode = DepthTest::None;

		cmd.drawables.emplace_back(new Primitive(m_fullscreenTriangle, tonemappingMat));

		m_renderPassList.emplace_back(
			new RenderPass(std::move(cmd)));
	}
}

IRenderTargetSPtr RenderEngine::renderTarget() const
{
	return m_outputTarget;
}

void RenderEngine::setRenderingScale(double scale)
{
	m_scale = scale;
}

const std::list<RenderPassSPtr>& RenderEngine::renderPasses() const
{
	return m_renderPassList;
}

void RenderEngine::setupShadowMapping()
{
	std::vector<IDrawableSPtr> shadowCasters;
	shadowCasters.reserve(m_scene->nodeNum());

	auto t = m_scene->traverser();
	while (t.hasNext())
	{
		IDrawableSPtr drawable = t.next();
		if (drawable->geometry())
		{
			shadowCasters.push_back(drawable);
		}
	}

	const int shadowMapWidth = 1024;
	const int shadowMapHeight = 1024;

	const TextureSampler depthMapSampler = { 
		TextureFilter::Linear, 
		TextureWrap::ClampToBorder, 
		false, glm::vec4(1,1,1,1) 
	};

	const glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0);

	BoundingBox sceneBounds = m_scene->sceneBounds();

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

		if (!m_api->allocate(shadowMapTarget))
		{
			continue;
		}

		DirectionalLightSPtr dirLight = std::static_pointer_cast<DirectionalLight>(light);

		// create light Camera
		const glm::vec3 lightDirection = dirLight->direction();
		glm::mat4 lightView = glm::lookAt(
			sceneBounds.center(),
			sceneBounds.center() + lightDirection,
			glm::vec3(0, 1, 0));

		BoundingBox cameraVolume = lightView * sceneBounds;

		glm::mat4 lightProj = glm::ortho(
			cameraVolume.min().x, cameraVolume.max().x,
			cameraVolume.min().y, cameraVolume.max().y,
			cameraVolume.min().z, cameraVolume.max().z );

		glm::mat4 worldToLight = lightProj * lightView;

		MaterialSPtr shadowMat = m_matlib->instanciate("Util.ShadowMapping");
		shadowMat->setUniform("worldToLight", worldToLight);

		RenderCommand cmd;
		cmd.name = "ShadowMapping";
		cmd.target = shadowMapTarget;
		cmd.state.clearColor = false;
		cmd.state.writeColor = false;
		cmd.state.depthOffset = glm::vec2(4., 1.);
		cmd.overrideMaterial = shadowMat;
		cmd.drawables = shadowCasters;
		m_renderPassList.emplace_back(
			new RenderPass(std::move(cmd)));

		ShadowData sData = ShadowData();
		sData.index = index;
		sData.lightMatrice = biasMatrix * worldToLight;
		sData.data = shadowMap;
		sData.material = shadowMat;
		m_shadowData[dirLight] = std::move(sData);

		++index;
	}
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
		
		ShadowData shadowData = ShadowData();
		auto found = m_shadowData.find(light);
		if (found != m_shadowData.end())
		{
			shadowData = found->second;
		}
		// TODO update shadow matrix/material if light transform changed!
		data.lightsMatrix[data.numLights] = shadowData.lightMatrice;
		data.shadowMapIndex[data.numLights] = shadowData.index;
		
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
	m_api->allocate(rt);
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

	MaterialSPtr projectionMat = m_matlib->instanciate("Util.ProjectEqr2Cube", "ProjecEqr2Cube");
	if (!projectionMat)
	{
		return;
	}

	const int rotXDeg = 0;
	projectionMat->setUniform("horizontalRotation", (rotXDeg % 360) / 360.f);
	projectionMat->setUniform("equirectangularMap", source);

	// TODO defaults, store somewhere else
	const glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.f, .1f, 10.f);
	const glm::mat4 views[6] = {
		glm::lookAt(glm::vec3(0,0,0), glm::vec3( 1, 0, 0), glm::vec3( 0,-1, 0)),
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(-1, 0, 0), glm::vec3( 0,-1, 0)),
		glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0, 1, 0), glm::vec3( 0, 0, 1)),
		glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0,-1, 0), glm::vec3( 0, 0,-1)),
		glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0, 0, 1), glm::vec3( 0,-1, 0)),
		glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0, 0,-1), glm::vec3( 0,-1, 0))
	};

	for (int i = 0; i < 6; ++i)
	{
		projectionMat->setUniform("VP[" + std::to_string(i) + "]", proj * views[i]);
	}

	RenderCommand cmd;
	cmd.name = "EquirectangularToCubemap";
	cmd.target = rt;
	cmd.drawables.emplace_back(new Primitive(cubeGeom, projectionMat));

	cmd.state.clearColor = true;
	cmd.state.color = glm::vec4(0, 0, 0, 1);
	cmd.state.cullingMode = Culling::Front;
	cmd.state.depthTestMode = DepthTest::Equal;
	cmd.state.writeDepth = false;

	render(RenderPass(std::move(cmd)));

	m_renderer->regenerateMipmaps(target);
}
