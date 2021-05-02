#pragma once

#include <string>

enum class ShaderType : int
{
	Vertex,
	Fragment,
	Geometry,
	Compute
};

class ShaderSource
{
public:

	ShaderSource(ShaderType type, const std::string& source);

	ShaderSource(ShaderType type, std::string&& source);

	~ShaderSource();

	const ShaderType type() const;

	const std::string source() const;

protected:

	ShaderType m_type;
	std::string m_source;

};

