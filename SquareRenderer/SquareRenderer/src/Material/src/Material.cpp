#include "Material/Material.h"
#include "Texture/ITexture.h"

Material::Material(const std::string& name, ShaderProgramSPtr program)
	: m_name(name)
	, m_program(program)
{
}

ShaderProgramSPtr Material::program() const
{
	return m_program;
}

void Material::setProgram(ShaderProgramSPtr program)
{
	unbind();
	m_program = program;
}

void Material::setLayer(Material::Layer layer)
{
	m_layer = layer;
}

Material::Layer Material::layer() const
{
	return m_layer;
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

bool Material::setUniform(const std::string& name, const UniformValue& value)
{
	if (isBound())
	{
		return m_program->setUniform(name, value);
	}
	else
	{
		m_uniformStorage[name] = value;
		return true;
	}
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

bool Material::setUniform(const std::string& name, ITextureSPtr value)
{
	/*const int activeTextureSlot = static_cast<int>(m_uniformStorage.size());
	if (setUniform(name, activeTextureSlot))
	{
		m_textureStorage[name] = value;
		return true;
	}*/
	if (!isBound() && value)
	{
		m_textureStorage[name] = value;
		return true;
	}
	return false;
}

UniformValue Material::uniformValue(const std::string& name) const
{
	const auto& found = m_uniformStorage.find(name);
	if (found != m_uniformStorage.end())
	{
		return found->second;
	}

	return UniformValue();
}

const std::unordered_map<std::string, ITextureSPtr>& Material::uniformTextures() const
{
	return m_textureStorage;
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
