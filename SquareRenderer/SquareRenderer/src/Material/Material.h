#pragma once

#include "Common/Macros.h"
#include "Material/ShaderProgram.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>

DECLARE_PTRS(Material);
DECLARE_PTRS(ITexture);

class Material
{
public:

	Material(const std::string& name, ShaderProgramSPtr program);
	Material(std::string&& name, ShaderProgramSPtr program);

	ShaderProgramSPtr program() const;

	const std::string& name() const;

	void bind();

	void unbind() const;

	bool isBound() const;

	bool setUniform(const std::string& name, UniformValue&& value);
	bool setUniform(const std::string& name, ITextureSPtr value);

	UniformValue uniformValue(const std::string& name) const;

	const std::unordered_map<std::string, ITextureSPtr>& uniformTextures() const;
	
private:

	std::string m_name;

	mutable bool m_isBound;

	ShaderProgramSPtr m_program;

	std::unordered_map<std::string, UniformValue> m_uniformStorage;

	std::unordered_map<std::string, ITextureSPtr> m_textureStorage;

	void setUniforms();
};
