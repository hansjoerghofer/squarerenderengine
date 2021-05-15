#include "Preprocessor/SceneImporter.h"
#include "Preprocessor/SceneLoader.h"
#include "Common/Logger.h"
#include "API/GraphicsAPI.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Material/MaterialLibrary.h"

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
	SceneLoader loader(m_matLib);
	loader.setDefaultProgramName("Lit.BlinnPhong");
	SceneUPtr scene = loader.loadFromFile(filepath);

	/*for (SceneElementSPtr element : scene->sceneElements())
	{
		if (!element->geometry()) continue;

		if (!m_api->allocate(*element->geometry()))
		{
			Logger::Warning("Could not allocate geometry buffers.");
		}
	}*/

	auto t = scene->traverser();
	while (t.hasNext())
	{
		SceneNodeSPtr node = t.next();
		if (node->geometry() && !m_api->allocate(*node->geometry()))
		{
			Logger::Warning("Could not allocate geometry buffers.");
		}
	}

	return scene;
}
