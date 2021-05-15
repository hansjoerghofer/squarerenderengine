#pragma once

#include "Common/Macros.h"

#include <string>
#include <vector>

DECLARE_PTRS(Scene);
DECLARE_PTRS(SceneNode);
DECLARE_PTRS(Mesh);
DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
struct aiMaterial;
struct aiMesh;
struct aiScene;
struct aiNode;

class SceneLoader
{
public:

	SceneLoader(MaterialLibrarySPtr matLib);

	~SceneLoader();

	void setDefaultProgramName(const std::string& defaultProgramName);

	SceneUPtr loadFromFile(const std::string& filepath);

private:

	MaterialSPtr processMaterial(const aiMaterial& mat);

	MeshSPtr processMesh(const aiMesh& mesh);

	SceneNodeSPtr processNode(
		const aiNode& currentNode,
		const std::vector<MaterialSPtr>& materialSet,
		const std::vector<std::pair<MeshSPtr, unsigned int>>& meshSet);

	MaterialLibrarySPtr m_matLib;

	std::string m_defaultProgramName;
};

