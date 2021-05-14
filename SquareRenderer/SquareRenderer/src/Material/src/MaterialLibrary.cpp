#include "Material/MaterialLibrary.h"
#include "Material/Material.h"
#include "Material/ShaderProgram.h"

#include <sstream>

void MaterialLibrary::registerProgram(ShaderProgramSPtr program)
{
	m_registeredPrograms[program->name()] = program;
}

ShaderProgramSPtr MaterialLibrary::findProgram(const std::string& name)
{
	return m_registeredPrograms[name];
}

const std::unordered_map<std::string, ShaderProgramSPtr>& MaterialLibrary::programs() const
{
	return m_registeredPrograms;
}

MaterialUPtr MaterialLibrary::instanciate(const std::string& programName) const
{
	auto found = m_registeredPrograms.find(programName);
	if (found != m_registeredPrograms.end())
	{
		ShaderProgramSPtr program = found->second;

		std::stringstream name(program->name());
		name << "_mat(" << program.use_count() << ")";

		return MaterialUPtr(new Material(name.str(), program));
	}

	return MaterialUPtr();
}

MaterialLibraryUPtr MaterialLibrary::create()
{
	return std::make_unique<MaterialLibrary>();
}
