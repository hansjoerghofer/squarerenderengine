#include "Renderer/RenderEngine.h"
#include "API/GraphicsAPI.h"
#include "Scene/Scene.h"
#include "Scene/SceneElement.h"
#include "Scene/Geometry.h"
#include "Material/Material.h"
#include "Material/ShaderProgram.h"
#include "Material/UniformBlock.h"
#include "Renderer/Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CameraUniformBlock
{
	alignas(16) glm::mat4 P;    // projection mat
	alignas(16) glm::mat4 invP; // inverse projection mat
	alignas(16) glm::mat4 V;    // view mat
	alignas(16) glm::mat4 invV; // inverse view mat
	alignas(16) glm::mat4 VP;   // view-projection mat
	alignas(16) glm::vec4 dim;  // resolution (w,h,1/w,1/h)
	alignas(16) glm::vec4 clip;  // clipping planes (n,f,1/n,1/f)
};

constexpr auto MAX_LIGHT_COUNT = 4;
struct LightsUniformBlock
{
	alignas(16) glm::vec4 ambientColor;

	alignas(16) int numLights;
	alignas(16) glm::vec4 lightsPosWS[MAX_LIGHT_COUNT];
	alignas(16) glm::vec4 lightsColor[MAX_LIGHT_COUNT];
};

RenderEngine::RenderEngine(GraphicsAPISPtr api)
	: m_api(api)
	, m_renderer(new Renderer())
	, m_cameraUniformData(api->allocateUniformBlock(0, sizeof(CameraUniformBlock)))
	, m_lightsUniformData(api->allocateUniformBlock(1, sizeof(LightsUniformBlock)))
{
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::setScene(SceneSPtr scene)
{
	m_scene = scene;

	// TODO outsource logic!
	for (SceneElementSPtr elem : m_scene->sceneElements())
	{
		if (!elem->material()->program()
			->bindUniformBlock("CameraUBO", m_cameraUniformData->bindingPoint()))
		{
			Logger::Warning("Cannot bind CameraUBO to %s", 
				elem->material()->program()->name().c_str());
		}

		if (!elem->material()->program()
			->bindUniformBlock("LightsUBO", m_lightsUniformData->bindingPoint()))
		{
			Logger::Warning("Cannot bind LightsUBO to %s",
				elem->material()->program()->name().c_str());
		}
	}
}

void RenderEngine::render(const Camera& camera)
{
	updateCameraUniformData(camera);
	updateLightsUniformData();

	RendererState state;
	state.clearColor = true;
	//state.enableWireframe = true;
	state.color = glm::vec4(.6f, .6f, .8f, 1);

	m_renderer->setupView(camera);
	m_renderer->applyState(state);

	for (SceneElementSPtr elem : m_scene->sceneElements())
	{
		m_renderer->render(*elem->geometry(), *elem->material());
	}
}

void RenderEngine::renderLines(const Camera& camera, Geometry& pointset, ShaderProgram& progam)
{
	updateCameraUniformData(camera);

	// TODO outsource logic!
	if (!progam.bindUniformBlock("CameraUBO", m_cameraUniformData->bindingPoint()))
	{
		Logger::Warning("Cannot bind CameraUBO to %s", progam.name().c_str());
	}

	RendererState state;
	state.clearColor = false;
	state.clearDepth = false;
	state.writeDepth = false;
	state.cullingMode = Culling::None;

	m_renderer->setupView(camera);
	m_renderer->applyState(state);

	m_renderer->renderLines(pointset, progam);
}

void RenderEngine::updateCameraUniformData(const Camera& camera)
{
	// TODO check if dirty

	CameraUniformBlock data = CameraUniformBlock();
	data.P = camera.projectionMatrix();
	data.V = camera.viewMatrix();

	data.invP = glm::inverse(data.P);
	data.invV = glm::inverse(data.V);

	data.VP = data.P * data.V;

	data.dim.x = static_cast<float>(camera.width());
	data.dim.y = static_cast<float>(camera.height());
	data.dim.z = 1.f / data.dim.x;
	data.dim.w = 1.f / data.dim.y;

	data.clip.x = camera.near();
	data.clip.y = camera.far();
	data.clip.z = 1.f / data.clip.x;
	data.clip.w = 1.f / data.clip.y;

	if (m_cameraUniformData && m_cameraUniformData->isValid())
	{
		m_cameraUniformData->update(reinterpret_cast<const char*>(&data));
	}
}

void RenderEngine::updateLightsUniformData()
{
	// TODO check if dirty

	LightsUniformBlock data = LightsUniformBlock();
	data.ambientColor = glm::vec4(.5, .5, .5, 1);
	data.numLights = 0;
	
	data.lightsPosWS[data.numLights] = glm::vec4(1, 1, 1, 1);
	data.lightsColor[data.numLights] = glm::vec4(0.5, 0.5, 1, 1);
	data.numLights++;

	data.lightsPosWS[data.numLights] = glm::normalize(glm::vec4(1, -1, -0.5, 0));
	data.lightsColor[data.numLights] = glm::vec4(0.5, 1, 0.5, 1);
	data.numLights++;

	if (m_lightsUniformData && m_lightsUniformData->isValid())
	{
		m_lightsUniformData->update(reinterpret_cast<const char*>(&data));
	}
}
