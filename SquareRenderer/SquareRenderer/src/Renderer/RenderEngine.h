#pragma once

#include "Common/Macros.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#include <list>

DECLARE_PTRS(Scene);
DECLARE_PTRS(Camera);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(UniformBlockResource);

class RenderEngine
{
public:

	RenderEngine(GraphicsAPISPtr api, MaterialLibrarySPtr matlib);

	~RenderEngine();

	void setScene(SceneSPtr scene);

	void setMainCamera(CameraSPtr camera);

	void setupGizmos(const std::string& programName);

	void render();
	
protected:

	GraphicsAPISPtr m_api;

	MaterialLibrarySPtr m_matlib;

	RendererUPtr m_renderer;

	SceneSPtr m_scene;

	CameraSPtr m_mainCamera;

	GeometrySPtr m_gizmoGeometry;
	MaterialSPtr m_gizmoMaterial;

	UniformBlockResourceUPtr m_cameraUniformData;
	UniformBlockResourceUPtr m_lightsUniformData;

	bool m_showGizmos = true;

	std::list<RenderCommand> m_renderCommandList;

	void rebuildCommandList();

	void updateCameraUniformData(const Camera& camera);
	void updateLightsUniformData();
};

