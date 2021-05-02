#pragma once

#include "Common/Macros.h"

#include <string>
#include <unordered_map>
#include <vector>

DECLARE_PTRS(ShaderSource);
enum class ShaderType : int;

class ShaderProgram
{
public:

	ShaderProgram(const std::string& name);

	const std::string name() const;

	std::vector<ShaderSourceSPtr> sources() const;

	void addShaderSource(ShaderSourceSPtr source);

protected:

	std::string m_name;
	std::unordered_map<ShaderType, ShaderSourceSPtr> m_sources;
};

