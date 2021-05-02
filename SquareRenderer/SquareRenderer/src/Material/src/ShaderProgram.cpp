#include "Material/ShaderProgram.h"
#include "Material/ShaderSource.h"

ShaderProgram::ShaderProgram(const std::string& name)
	: m_name(name)
{
}

const std::string ShaderProgram::name() const
{
	return m_name;
}

void ShaderProgram::addShaderSource(ShaderSourceSPtr source)
{
	m_sources[source->type()] = source;
}

std::vector<ShaderSourceSPtr> ShaderProgram::sources() const
{
	std::vector<std::shared_ptr<ShaderSource>> out;
	out.reserve(m_sources.size());

	for (const auto& entry : m_sources)
	{
		out.emplace_back(entry.second);
	}

	return out;
}
