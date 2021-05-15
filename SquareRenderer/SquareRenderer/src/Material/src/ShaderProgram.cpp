#include "Material/ShaderProgram.h"
#include "Material/ShaderSource.h"

ShaderProgram::ShaderProgram(const std::string& name)
	: m_name(name)
	, m_isBound(false)
{
}

ShaderProgram::ShaderProgram(std::string&& name)
	: m_name(std::move(name))
	, m_isBound(false)
{
}

const std::string& ShaderProgram::name() const
{
	return m_name;
}

void ShaderProgram::link(ShaderProgramResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

int ShaderProgram::id() const
{
	if (m_linkedResource && m_linkedResource->isValid())
	{
		return static_cast<int>(m_linkedResource->handle());
	}
	
	return -1;
}

void ShaderProgram::bind()
{
	if (!m_isBound 
		&& m_linkedResource 
		&& m_linkedResource->isValid())
	{
		m_linkedResource->bind();
		m_isBound = true;

		resetToDefaults();
	}
}

void ShaderProgram::unbind()
{
	if (m_isBound
		&& m_linkedResource
		&& m_linkedResource->isValid())
	{
		m_linkedResource->unbind();
		m_isBound = false;
	}
}

bool ShaderProgram::isBound() const
{
	return m_isBound && m_linkedResource && m_linkedResource->isValid();
}

void ShaderProgram::addShaderSource(ShaderSourceSPtr source)
{
	if (!source)
	{
		return;
	}

	// check if shader replaces another one of the same type
	for (size_t i = 0; i < m_sources.size(); ++i)
	{
		if (m_sources[i]->type() == source->type())
		{
			m_sources[i] = source;
			return;
		}
	}

	// add shader to list
	m_sources.push_back(source);
}

template<typename T>
bool ShaderProgram::setGenericUniform(int location, const T& value)
{
	if (isBound() && location != -1)
	{
		m_linkedResource->setUniform(location, value);
		return true;
	}
	return false;
}

bool ShaderProgram::setUniform(int location, const UniformValue& value)
{
	switch (value.type)
	{
	case UniformValue::Type::Int:
		return setGenericUniform(location, std::any_cast<int>(value.value));
	case UniformValue::Type::UInt:
		return setGenericUniform(location, std::any_cast<unsigned int>(value.value));
	case UniformValue::Type::Float:
		return setGenericUniform(location, std::any_cast<float>(value.value));
	case UniformValue::Type::Vec4:
		return setGenericUniform(location, std::any_cast<glm::vec4>(value.value));
	case UniformValue::Type::Mat4x4:
		return setGenericUniform(location, std::any_cast<glm::mat4>(value.value));
	default:
		return false;
	}
}

bool ShaderProgram::setUniform(const std::string& name, const UniformValue& value)
{
	const int location = fetchUniformLocation(name);
	return setUniform(location, value);
}

bool ShaderProgram::setUniformDefault(const std::string& name, UniformValue&& value)
{
	if (m_linkedResource && m_linkedResource->isValid())
	{
		const int location = fetchUniformLocation(name);

		if (location != -1)
		{
			m_defaultUniformStorage[location] = std::move(value);
			return true;
		}
	}

	return false;
}

bool ShaderProgram::bindUniformBlock(const std::string& name, int bindingPoint)
{
	if (m_linkedResource && m_linkedResource->isValid())
	{
		return m_linkedResource->bindUniformBlock(name, bindingPoint);
	}

	return false;
}

int ShaderProgram::fetchUniformLocation(const std::string& name)
{
	const auto& found = m_nameToUniformLocation.find(name);
	if (found != m_nameToUniformLocation.end())
	{
		return found->second;
	}
	else if(m_linkedResource && m_linkedResource->isValid())
	{
		const int location = m_linkedResource->uniformLocation(name);
		if (location != -1)
		{
			m_nameToUniformLocation[name] = location;
			return location;
		}
	}

	return -1;
}

void ShaderProgram::resetToDefaults()
{
	for (const auto& [location, value] : m_defaultUniformStorage)
	{
		setUniform(location, value);
	}
}

const std::vector<ShaderSourceSPtr>& ShaderProgram::sources() const
{
	return m_sources;
}
