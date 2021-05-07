#pragma once

#include "Common/Macros.h"

#include <glm/glm.hpp>

#include <unordered_map>

struct UniformValue;
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(Material);

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
	
private:

	std::string m_name;

	mutable bool m_isBound;

	ShaderProgramSPtr m_program;

	std::unordered_map<std::string, UniformValue> m_uniformStorage;

	void setUniforms();
};
