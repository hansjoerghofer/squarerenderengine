#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(Scene);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(MeshImporter);
DECLARE_PTRS(RenderEngine);

class SceneImporter
{
public:

	explicit SceneImporter(GraphicsAPISPtr api, MaterialLibrarySPtr matLib, RenderEngineSPtr renderEngine);

	~SceneImporter();

	SceneUPtr importFromFile(const std::string& filepath);

private:

	GraphicsAPISPtr m_api;

	MaterialLibrarySPtr m_matLib;

	RenderEngineSPtr m_renderEngine;
};

