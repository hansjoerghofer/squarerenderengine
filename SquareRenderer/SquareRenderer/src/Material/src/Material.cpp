#include "Material/Material.h"
#include "Material/ShaderProgram.h"

Material::Material(const std::string& name, ShaderProgramSPtr program)
	: m_name(name)
	, m_program(program)
	, m_isBound(false)
{
}

Material::Material(std::string&& name, ShaderProgramSPtr program)
	: m_name(std::move(name))
	, m_program(program)
	, m_isBound(false)
{
}

ShaderProgramSPtr Material::program() const
{
	return m_program;
}

const std::string& Material::name() const
{
	return m_name;
}

void Material::bind()
{
	if (!m_isBound && m_program)
	{
		m_program->bind();
		m_isBound = true;

		setUniforms();
	}
}

void Material::unbind() const
{
	if (m_isBound && m_program)
	{
		m_program->unbind();
		m_isBound = false;
	}
}

bool Material::isBound() const
{
	return m_isBound;
}

bool Material::setUniform(const std::string& name, UniformValue&& value)
{
	if (isBound())
	{
		return m_program->setUniform(name, std::move(value));
	}
	else
	{
		m_uniformStorage[name] = std::move(value);
		return true;
	}
}

void Material::setUniforms()
{
	if (!isBound())
	{
		return;
	}

	for (const auto& [name, value] : m_uniformStorage)
	{
		m_program->setUniform(name, value);
	}
}
