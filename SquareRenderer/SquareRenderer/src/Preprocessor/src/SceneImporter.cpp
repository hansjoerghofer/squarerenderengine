#include "Preprocessor/SceneImporter.h"
#include "Preprocessor/SceneLoader.h"
#include "Common/Logger.h"
#include "API/GraphicsAPI.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/IGeometry.h"
#include "Material/MaterialLibrary.h"

#include <filesystem>

SceneImporter::SceneImporter(GraphicsAPISPtr api, MaterialLibrarySPtr matLib)
	: m_api(api)
	, m_matLib(matLib)
{
}

SceneImporter::~SceneImporter()
{
}

SceneUPtr SceneImporter::importFromFile(const std::string& filepath)
{
	const std::filesystem::path path(filepath);

	if (!std::filesystem::exists(path) || !path.has_filename())
	{
		throw std::runtime_error("Invalid path to scene file.");
	}

	SceneLoader loader(m_matLib);
	loader.setDefaultProgramName("Lit.PBR");
	SceneUPtr scene = loader.loadFromFile(filepath);

	auto t = scene->traverser();
	while (t.hasNext())
	{
		SceneNodeSPtr node = t.next();
		IGeometrySPtr geo = node->geometry() ? 
			std::dynamic_pointer_cast<IGeometry>(node->geometry()) : nullptr;

		if (geo && !m_api->allocate(geo))
		{
			Logger::Warning("Could not allocate geometry buffers.");
		}
	}

	return scene;
}
