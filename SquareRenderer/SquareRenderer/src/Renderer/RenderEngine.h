#pragma once

#include "Common/Macros.h"
#include "Renderer/Renderer.h"
#include "Renderer/UniformBlockData.h"

#include <vector>
#include <unordered_map>

DECLARE_PTRS(Scene);
DECLARE_PTRS(Camera);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(ILightsource);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IDrawable);
DECLARE_PTRS(Texture2D);
DECLARE_PTRS(Cubemap);
DECLARE_PTRS(IRenderPass);
DECLARE_PTRS(ResourceManager);
DECLARE_PTRS(ShadowMappingRenderPass);

template<typename T>
class UniformBlockData;
struct CameraUniformBlock;
struct LightsUniformBlock;
struct RenderCommand;

struct IBLData
{
	CubemapSPtr diffuse;
	CubemapSPtr specular;
	Texture2DSPtr brdf;
};

class RenderEngine
{
public:

	RenderEngine(GraphicsAPISPtr api, MaterialLibrarySPtr matlib);

	~RenderEngine();

	void setScene(SceneSPtr scene);

	void setMainCamera(CameraSPtr camera);

	void setRenderTarget(IRenderTargetSPtr target);

	IRenderTargetSPtr renderTarget() const;

	void setRenderingScale(double scale);

	void setupGizmos(const std::string& programName);

	const std::vector<IRenderPassSPtr>& renderPasses() const;

	void update(double deltaTime);

	void render();

	/* Helper Functions */

	void projectEquirectangularToCubemap(Texture2DSPtr source, CubemapSPtr target);

	IBLData generateIBL(CubemapSPtr hdri);

	void hdriToDiffuseIrradiance(CubemapSPtr hdri, CubemapSPtr diffIrradiance);

	void hdriToSpecularIrradiance(CubemapSPtr hdri, CubemapSPtr specIrradiance);
	
	void generateIntegratedBRDF(Texture2DSPtr integratedBRDF);

	void packTextures(
		Texture2DSPtr target, 
		Texture2DSPtr sourceRed, 
		Texture2DSPtr sourceGreen, 
		Texture2DSPtr sourceBlue, 
		Texture2DSPtr sourceAlpha);

protected:

	void setupPostProcessing();

	GraphicsAPISPtr m_api;

	MaterialLibrarySPtr m_matlib;

	RendererUPtr m_renderer;

	SceneSPtr m_scene;

	CameraSPtr m_mainCamera;

	IRenderTargetSPtr m_outputTarget;

	RenderTargetSPtr m_thinGBuffer;

	RenderTargetSPtr m_colorBuffer;

	IDrawableSPtr m_gizmos;

	ResourceManagerSPtr m_resources;

	std::shared_ptr<UniformBlockData<CameraUniformBlock>> m_cameraUniformBlock;
	std::shared_ptr<UniformBlockData<LightsUniformBlock>> m_lightsUniformBlock;

	bool m_showGizmos = true;

	double m_scale = 1.0;

	std::vector<IRenderPassSPtr> m_renderPassList;

	ShadowMappingRenderPassSPtr m_shadowMapping;

	IBLData m_ibl;

	void rebuildCommandList();

	void updateCameraUniformData(CameraSPtr camera);
	void updateLightsUniformData();
};

