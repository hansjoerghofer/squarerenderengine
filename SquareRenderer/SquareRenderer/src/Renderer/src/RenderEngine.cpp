#include "Renderer/RenderEngine.h"
#include "Common/Logger.h"
#include "API/GraphicsAPI.h"
#include "Scene/Scene.h"
#include "Scene/SceneElement.h"
#include "Scene/Geometry.h"
#include "Scene/LineSet.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
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

RenderEngine::RenderEngine(GraphicsAPISPtr api, MaterialLibrarySPtr matlib)
	: m_api(api)
	, m_matlib(matlib)
	, m_renderer(new Renderer())
	, m_cameraUniformData(api->allocateUniformBlock(0, sizeof(CameraUniformBlock)))
	, m_lightsUniformData(api->allocateUniformBlock(1, sizeof(LightsUniformBlock)))
{
	for (auto& [name, program] : m_matlib->programs())
	{
		const auto result = api->compile(*program);
		if (!result)
		{
			Logger::Error("Error while linking shader program: '%s'\n%s",
				program->name().c_str(), result.message.c_str());

			continue;
		}

		// bind uniform blocks to all programs who need it
		program->bindUniformBlock("CameraUBO", m_cameraUniformData->bindingPoint());
		program->bindUniformBlock("LightsUBO", m_lightsUniformData->bindingPoint());
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

void RenderEngine::render()
{
	updateLightsUniformData();

	CameraSPtr cachedCam;
	for (const RenderCommand& cmd : m_renderCommandList)
	{
		if (cmd.camera != cachedCam)
		{
			updateCameraUniformData(*cmd.camera);
			m_renderer->setupView(*cmd.camera);
			cachedCam = cmd.camera;
		}

		m_renderer->applyState(cmd.state);

		for (const RenderElement& elem : cmd.elements)
		{
			// TODO override logic!
			MaterialSPtr mat = cmd.overrideMaterial ? cmd.overrideMaterial : elem.material;
			if (mat)
			{
				m_renderer->render(*elem.geometry, *mat);
			}
		}
	}
}

void RenderEngine::rebuildCommandList()
{
	m_renderCommandList.clear();

	if (!m_mainCamera)
	{
		return;
	}

	// scene
	if (m_scene)
	{
		RenderCommand cmd;
		cmd.camera = m_mainCamera;
		cmd.state.clearColor = true;
		//cmd.state.enableWireframe = true;
		cmd.state.color = glm::vec4(.6f, .6f, .8f, 1);

		cmd.elements.reserve(m_scene->sceneElements().size());
		for (SceneElementSPtr elem : m_scene->sceneElements())
		{
			cmd.elements.emplace_back(elem->geometry(), elem->material());
		}

		m_renderCommandList.emplace_back(std::move(cmd));
	}

	if(m_gizmoGeometry)
	{
		RenderCommand cmd;
		cmd.camera = m_mainCamera;
		cmd.state.clearColor = false;
		cmd.state.clearDepth = false;
		cmd.state.writeDepth = false;
		cmd.state.cullingMode = Culling::None;
		cmd.state.primitive = PrimitiveMode::Lines;

		cmd.elements.emplace_back(m_gizmoGeometry, m_gizmoMaterial);

		m_renderCommandList.emplace_back(std::move(cmd));
	}
}

void RenderEngine::setupGizmos(const std::string& programName)
{
	m_gizmoMaterial = m_matlib->instanciate(programName);
	if (!m_gizmoMaterial)
	{
		Logger::Error("Error could not find shader program '%s'.", programName.c_str());
		return;
	}

	constexpr glm::vec3 origin(0, 0, 0);
	constexpr glm::vec3 axisX(1, 0, 0);
	constexpr glm::vec3 axisY(0, 1, 0);
	constexpr glm::vec3 axisZ(0, 0, 1);

	m_gizmoGeometry.reset(new LineSet({
			origin, axisX,
			origin, axisY,
			origin, axisZ
		}, { axisX, axisY, axisZ }));

	if (!m_api->allocate(*m_gizmoGeometry))
	{
		Logger::Error("Error while allocating gizmo geometry buffers.");

		m_gizmoGeometry.reset();
	}

	rebuildCommandList();
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
