#include "Material/ShaderSource.h"
#include "API/SharedResource.h"

ShaderSource::ShaderSource(ShaderType type, const std::string& source,
	const std::vector<UniformMetaInfo>& uniformMetaInfo)
	: m_type(type)
	, m_source(source)
	, m_uniformMetaInfo(uniformMetaInfo)
{
}

ShaderSource::~ShaderSource()
{
}

void ShaderSource::link(IShaderResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

int ShaderSource::id() const
{
	if (m_linkedResource && m_linkedResource->isValid())
	{
		return static_cast<int>(m_linkedResource->handle());
	}

	return -1;
}

ShaderType ShaderSource::type() const
{
	return m_type;
}

const std::vector<UniformMetaInfo>& ShaderSource::uniformMetaInfo() const
{
	return m_uniformMetaInfo;
}

const std::string& ShaderSource::source() const
{
	return m_source;
}
