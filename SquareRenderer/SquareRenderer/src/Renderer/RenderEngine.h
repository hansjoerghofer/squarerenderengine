#pragma once

#include "Common/Macros.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/UniformBlockData.h"

#include <list>

DECLARE_PTRS(Scene);
DECLARE_PTRS(Camera);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(MaterialLibrary);

template<typename T>
class UniformBlockData;
struct CameraUniformBlock;
struct LightsUniformBlock;

class RenderEngine
{
public:

	RenderEngine(GraphicsAPISPtr api, MaterialLibrarySPtr matlib);

	~RenderEngine();

	void setScene(SceneSPtr scene);

	void setMainCamera(CameraSPtr camera);

	void setRenderTarget(IRenderTargetSPtr target);

	void setupGizmos(const std::string& programName);

	void update(double deltaTime);

	void render();
	
protected:

	void setupPostProcessing();

	GraphicsAPISPtr m_api;

	MaterialLibrarySPtr m_matlib;

	RendererUPtr m_renderer;

	SceneSPtr m_scene;

	CameraSPtr m_mainCamera;

	IRenderTargetSPtr m_outputTarget;

	RenderTargetSPtr m_gBuffer;

	IDrawableSPtr m_gizmos;

	IGeometrySPtr m_fullscreenTriangle;

	std::shared_ptr<UniformBlockData<CameraUniformBlock>> m_cameraUniformBlock;
	std::shared_ptr<UniformBlockData<LightsUniformBlock>> m_lightsUniformBlock;

	bool m_showGizmos = true;

	std::list<RenderCommand> m_renderCommandList;

	void rebuildCommandList();

	void updateCameraUniformData(CameraSPtr camera);
	void updateLightsUniformData();
};

