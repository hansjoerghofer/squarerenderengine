#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(SharedResource);
DECLARE_PTRS(ShaderSource);

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

	virtual ~ShaderSource();

	void link(SharedResourceUPtr resource);

	int id() const;

	ShaderType type() const;

	const std::string& source() const;

protected:

	ShaderType m_type;

	std::string m_source;

	SharedResourceUPtr m_linkedResource;

	std::string m_compileLog;
};

