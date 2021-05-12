#pragma once

#include "Common/Macros.h"
#include "Renderer/Renderer.h"

DECLARE_PTRS(Scene);
DECLARE_PTRS(Camera);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(UniformBlockResource);

class RenderEngine
{
public:

	explicit RenderEngine(GraphicsAPISPtr api);

	~RenderEngine();

	void setScene(SceneSPtr scene);

	void render(const Camera& camera);

	void renderLines(const Camera& camera, Geometry& pointset, ShaderProgram& progam);
	
protected:

	GraphicsAPISPtr m_api;

	SceneSPtr m_scene;

	RendererUPtr m_renderer;

	UniformBlockResourceUPtr m_cameraUniformData;
	UniformBlockResourceUPtr m_lightsUniformData;

	void updateCameraUniformData(const Camera& camera);
	void updateLightsUniformData();
};

