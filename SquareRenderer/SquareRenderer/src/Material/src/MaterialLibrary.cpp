#include "Material/MaterialLibrary.h"
#include "Material/Material.h"
#include "Material/ShaderProgram.h"

#include <sstream>

void MaterialLibrary::registerProgram(ShaderProgramSPtr program)
{
	m_registeredPrograms[program->name()] = program;
}

ShaderProgramSPtr MaterialLibrary::findProgram(const std::string& name) const
{
	auto found = m_registeredPrograms.find(name);
	if (found != m_registeredPrograms.end())
	{
		return found->second;
	}
	return nullptr;
}

MaterialSPtr MaterialLibrary::findMaterial(const std::string& name) const
{
	auto found = m_namedInstances.find(name);
	if (found != m_namedInstances.end())
	{
		return found->second;
	}
	return nullptr;
}

const std::unordered_map<std::string, ShaderProgramSPtr>& MaterialLibrary::programs() const
{
	return m_registeredPrograms;
}

MaterialUPtr MaterialLibrary::instanciate(const std::string& programName) const
{
	ShaderProgramSPtr program = findProgram(programName);
	if (program)
	{
		std::stringstream name(program->name());
		name << "_mat(" << program.use_count() << ")";

		return MaterialUPtr(new Material(name.str(), program));
	}

	return nullptr;
}

MaterialSPtr MaterialLibrary::instanciate(const std::string& programName, const std::string& instanceName)
{
	ShaderProgramSPtr program = findProgram(programName);
	if (program)
	{
		MaterialSPtr mat = MaterialSPtr(new Material(instanceName, program));

		m_namedInstances[instanceName] = mat;

		return mat;
	}

	return nullptr;
}

MaterialLibraryUPtr MaterialLibrary::create()
{
	return std::make_unique<MaterialLibrary>();
}
