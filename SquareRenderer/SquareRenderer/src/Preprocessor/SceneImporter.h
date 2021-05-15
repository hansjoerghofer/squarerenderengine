#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(Scene);
DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(MeshImporter);

class SceneImporter
{
public:

	explicit SceneImporter(GraphicsAPISPtr api, MaterialLibrarySPtr matLib);

	~SceneImporter();

	SceneUPtr importFromFile(const std::string& filepath);

private:

	GraphicsAPISPtr m_api;

	MaterialLibrarySPtr m_matLib;
};

