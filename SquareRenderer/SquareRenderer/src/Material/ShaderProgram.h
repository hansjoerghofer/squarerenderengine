#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"
#include "Material/Uniform.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>

struct UniformValue;
DECLARE_PTRS(ShaderSource);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(ITexture);

class ShaderProgram
{
public:

	explicit ShaderProgram(const std::string& name);
	explicit ShaderProgram(std::string&& name);

	const std::string& name() const;

	void link(IShaderProgramResourceUPtr resource);

	int id() const;

	void bind();

	void unbind();

	const std::vector<ShaderSourceSPtr>& sources() const;

	const std::unordered_map<std::string, UniformMetaInfo>& uniformMetaInfo() const;

	const std::unordered_map<std::string, ITextureSPtr>& defaultTextures() const;

	void addShaderSource(ShaderSourceSPtr source);

	bool setUniform(const std::string& name, const UniformValue& value);

	bool setUniformDefault(const std::string& name, UniformValue&& value);
	bool setUniformDefault(const std::string& name, ITextureSPtr texture);

	bool bindUniformBlock(const std::string& name, int bindingPoint);

private:

	std::string m_name;

	IShaderProgramResourceUPtr m_linkedResource;

	bool m_isBound;

	std::vector<ShaderSourceSPtr> m_sources;

	std::unordered_map<int, UniformValue> m_defaultUniformStorage;

	std::unordered_map<std::string, ITextureSPtr> m_defaultUniformTextures;

	std::unordered_map<std::string, int> m_uniformLocationCache;

	std::unordered_map<std::string, UniformMetaInfo> m_nameToUniformMetaInfo;

	int fetchUniformLocation(const std::string& name);

	void resetToDefaults();

	bool setUniform(int location, const UniformValue& value);

	template<typename T>
	bool setGenericUniform(int location, const T& value);
};

