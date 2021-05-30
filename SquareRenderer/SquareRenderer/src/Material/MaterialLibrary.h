#pragma once

#include "Common/Macros.h"

#include <string>
#include <unordered_map>

DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(ITexture);

class MaterialLibrary
{
public:
	void registerProgram(ShaderProgramSPtr program);

	void registerDefaultTexture(const std::string& key, ITextureSPtr texture);

	ShaderProgramSPtr findProgram(const std::string& name) const;

	MaterialSPtr findMaterial(const std::string& name) const;

	const std::unordered_map<std::string, ShaderProgramSPtr>& programs() const;

	MaterialUPtr instanciate(const std::string& programName) const;

	MaterialSPtr instanciate(const std::string& programName, const std::string& instanceName);

	const std::unordered_map<std::string, MaterialSPtr>& materials() const;

	static MaterialLibraryUPtr create();

private:

	std::unordered_map<std::string, ShaderProgramSPtr> m_registeredPrograms;

	std::unordered_map<std::string, MaterialSPtr> m_namedInstances;

	std::unordered_map<std::string, ITextureSPtr> m_defaultTextures;
};