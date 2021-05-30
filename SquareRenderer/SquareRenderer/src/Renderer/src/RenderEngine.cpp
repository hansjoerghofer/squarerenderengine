#include "Renderer/RenderEngine.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/DepthBuffer.h"
#include "Common/Logger.h"
#include "API/GraphicsAPI.h"
#include "API/SharedResource.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/Geometry.h"
#include "Scene/Mesh.h"
#include "Scene/LineSet.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Material/ShaderProgram.h"
#include "Renderer/Camera.h"
#include "Renderer/Primitive.h"
#include "Renderer/UniformBlockData.h"
#include "Texture/Texture2D.h"

#include "UniformBlockDataStructs.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	if (m_renderCommandList.empty())
	{
		return;
	}

	updateLightsUniformData();
	updateCameraUniformData(m_mainCamera);

	for (const RenderCommand& cmd : m_renderCommandList)
	{
		m_renderer->setTarget(cmd.target);

		m_renderer->applyState(cmd.state);

		for (IDrawableSPtr elem : cmd.drawables)
		{
			// TODO override logic!
			MaterialSPtr mat = cmd.overrideMaterial ? cmd.overrideMaterial : elem->material();
			if (mat)
			{
				elem->preRender(mat);
				m_renderer->render(elem->geometry(), mat);
				elem->postRender();
			}
		}
	}
}

void RenderEngine::update(double /*deltaTime*/)
{
	if (!m_outputTarget || !m_gBuffer) return;

	if(m_gBuffer->width() != m_outputTarget->width() ||
	   m_gBuffer->height() != m_outputTarget->height())
	{
		m_gBuffer.reset();
		rebuildCommandList();
	}
}

void RenderEngine::rebuildCommandList()
{
	m_renderCommandList.clear();

	if (!m_mainCamera || !m_outputTarget)
	{
		return;
	}

	if (!m_gBuffer)
	{
		Texture2DSPtr colorBuffer = std::make_shared<Texture2D>(
			m_outputTarget->width(), m_outputTarget->height(),
			TextureFormat::RGBAFloat);
		m_gBuffer = std::make_shared<RenderTarget>(
			colorBuffer, DepthBufferFormat::Depth24Stencil8);

		if (!m_api->allocate(m_gBuffer))
		{
			Logger::Error("Invalid Framebuffer. Abort render command list creation.");
			return;
		}
	}

	// scene
	if (m_scene)
	{
		RenderCommand cmd;
		cmd.name = "Opaque Scene";
		cmd.target = m_gBuffer;
		cmd.state.clearColor = true;
		//cmd.state.enableWireframe = true;
		cmd.state.color = glm::vec4(.6f, .6f, .8f, 1);

		cmd.drawables.reserve(m_scene->nodeNum());

		auto t = m_scene->traverser();
		while (t.hasNext())
		{
			IDrawableSPtr drawable = t.next();
			if (drawable->geometry())
			{
				cmd.drawables.push_back(drawable);
			}
		}

		m_renderCommandList.emplace_back(std::move(cmd));
	}

	setupPostProcessing();

	if(m_gizmos)
	{
		RenderCommand cmd;
		cmd.name = "Gizmos";
		cmd.target = m_outputTarget;
		cmd.state.clearColor = false;
		cmd.state.clearDepth = false;
		cmd.state.writeDepth = false;
		cmd.state.cullingMode = Culling::None;
		cmd.state.primitive = PrimitiveMode::Lines;
		cmd.drawables.push_back(m_gizmos);

		m_renderCommandList.emplace_back(std::move(cmd));
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

	constexpr glm::vec3 origin(0, 0, 0);
	constexpr glm::vec3 axisX(1, 0, 0);
	constexpr glm::vec3 axisY(0, 1, 0);
	constexpr glm::vec3 axisZ(0, 0, 1);

	glm::vec3 lightDir = glm::normalize(glm::vec3(1, -1, -0.5));
	glm::vec3 lightPos = glm::vec3(0.2, 0.2, 0.7);

	GeometrySPtr gizmoGeometry = LineSetSPtr(new LineSet({
		// axis
		{ origin, axisX * 10.f, axisX },
		{ origin, axisY * 10.f, axisY },
		{ origin, axisZ * 10.f, axisZ },
		//dir light
		{ -lightDir, origin - lightDir * .8f, glm::vec3(1,1,0) },
		//point light
		{ lightPos - axisX * .05f, lightPos + axisX * .05f, glm::vec3(1,1,0) },
		{ lightPos - axisY * .05f, lightPos + axisY * .05f, glm::vec3(1,1,0) },
		{ lightPos - axisZ * .05f, lightPos + axisZ * .05f, glm::vec3(1,1,0) },
		}));

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

	if (!m_fullscreenTriangle)
	{
		GeometrySPtr fullscreenTriangle = MeshSPtr(new Mesh({
			{{-1.f,-1.f, 0.f}},
			{{ 3.f,-1.f, 0.f}},
			{{-1.f, 3.f, 0.f}}
			}, {}, false, false, false));

		if (!m_api->allocate(fullscreenTriangle))
		{
			Logger::Warning("Error while allocating fullscreen triangle buffer.");
			return;
		}

		m_fullscreenTriangle = fullscreenTriangle;
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

		m_renderCommandList.emplace_back(std::move(cmd));
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
	data.ambientColor = glm::vec4(.5, .5, .5, 1);
	data.numLights = 0;
	
	data.lightsPosWS[data.numLights] = glm::vec4(1, 1, 1, 1);
	data.lightsColor[data.numLights] = glm::vec4(0.2, 0.2, 0.7, 1);
	data.numLights++;

	data.lightsPosWS[data.numLights] = glm::normalize(glm::vec4(1, -1, -0.5, 0));
	data.lightsColor[data.numLights] = glm::vec4(0.2, 0.7, 0.1, 1);
	data.numLights++;

	m_lightsUniformBlock->update(data);
}
