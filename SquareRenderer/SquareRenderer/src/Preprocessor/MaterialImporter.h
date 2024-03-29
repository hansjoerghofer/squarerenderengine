#pragma once

#include "Common/Macros.h"

#include <string>
#include <vector>

DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(ShaderParser);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(ShaderProgram);

class MaterialImporter
{
public:

	explicit MaterialImporter(GraphicsAPISPtr api);

	~MaterialImporter();

	MaterialLibraryUPtr importFromFile(const std::string& filepath);

	ShaderProgramSPtr loadProgramFromFiles(
		const std::string& name, 
		const std::vector<std::string>& shaderPaths,
		const std::vector<std::string>& defines = {});

private:

	GraphicsAPISPtr m_api;

	ShaderParserUPtr m_shaderParser;
};

