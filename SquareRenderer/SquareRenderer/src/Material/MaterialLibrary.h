#pragma once

#include "Common/Macros.h"

#include <string>
#include <unordered_map>

DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(Material);
DECLARE_PTRS(MaterialLibrary);

class MaterialLibrary
{
public:
	void registerProgram(ShaderProgramSPtr program);

	ShaderProgramSPtr findProgram(const std::string& name);

	const std::unordered_map<std::string, ShaderProgramSPtr>& programs() const;

	MaterialUPtr instanciate(const std::string& programName) const;

	static MaterialLibraryUPtr create();

private:

	std::unordered_map<std::string, ShaderProgramSPtr> m_registeredPrograms;
};