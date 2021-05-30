#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"
#include "Material/Uniform.h"

#include <string>
#include <unordered_map>

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

	ShaderSource(ShaderType type, const std::string& source, 
		const std::vector<UniformMetaInfo >& uniformMetaInfo);

	virtual ~ShaderSource();

	void link(IShaderResourceUPtr resource);

	int id() const;

	ShaderType type() const;

	const std::vector<UniformMetaInfo>& uniformMetaInfo() const;

	const std::string& source() const;

protected:

	ShaderType m_type;

	std::string m_source;

	IShaderResourceUPtr m_linkedResource;

	std::string m_compileLog;

	std::vector<UniformMetaInfo> m_uniformMetaInfo;
};

