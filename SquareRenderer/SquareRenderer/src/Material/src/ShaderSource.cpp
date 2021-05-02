#include "Material/ShaderSource.h"

ShaderSource::ShaderSource(ShaderType type, const std::string& source)
	: m_type(type)
	, m_source(source)
{
}

ShaderSource::ShaderSource(ShaderType type, std::string&& source)
	: m_type(type)
	, m_source(source)
{
}

ShaderSource::~ShaderSource()
{
}

const ShaderType ShaderSource::type() const
{
	return m_type;
}

const std::string ShaderSource::source() const
{
	return m_source;
}
