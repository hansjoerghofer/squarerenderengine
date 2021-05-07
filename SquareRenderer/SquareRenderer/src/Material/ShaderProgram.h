#pragma once

#include "Common/Macros.h"
#include "Resources/SharedResource.h"

#include <glm/glm.hpp>

#include <any>
#include <string>
#include <vector>
#include <unordered_map>

struct UniformValue;
DECLARE_PTRS(ShaderSource);
DECLARE_PTRS(ShaderProgramResource);

class ShaderProgram
{
public:

	explicit ShaderProgram(const std::string& name);
	explicit ShaderProgram(std::string&& name);

	const std::string& name() const;

	void link(ShaderProgramResourceUPtr resource);

	int id() const;

	void bind();

	void unbind() const;

	bool isBound() const;

	const std::vector<ShaderSourceSPtr>& sources() const;

	void addShaderSource(ShaderSourceSPtr source);

	bool setUniform(const std::string& name, const UniformValue& value);

	bool setUniformDefault(const std::string& name, UniformValue&& value);

private:

	std::string m_name;

	ShaderProgramResourceUPtr m_linkedResource;

	mutable bool m_isBound;

	std::vector<ShaderSourceSPtr> m_sources;

	std::unordered_map<int, UniformValue> m_defaultUniformStorage;

	std::unordered_map<std::string, int> m_nameToUniformLocation;

	int fetchUniformLocation(const std::string& name);

	void resetToDefaults();

	bool setUniform(int location, const UniformValue& value);

	template<typename T>
	bool setGenericUniform(int location, const T& value);
};

struct UniformValue
{
	enum class Type
	{
		Int, UInt, Float, Vec4, Mat4x4
	};

	Type type;
	std::any value;

	UniformValue() : UniformValue(0) {}
	UniformValue(int v) : value(v), type(Type::Int) {}
	UniformValue(unsigned int v) : value(v), type(Type::UInt) {}
	UniformValue(float v) : value(v), type(Type::Float) {}
	UniformValue(const glm::vec4& v) : value(v), type(Type::Vec4) {}
	UniformValue(const glm::mat4x4& v) : value(v), type(Type::Mat4x4) {}
};

class ShaderProgramResource : public SharedResource
{
public:
	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual int uniformLocation(const std::string& name) const = 0;

	virtual void setUniform(int location, int value) const = 0;
	virtual void setUniform(int location, unsigned int value) const = 0;
	virtual void setUniform(int location, float value) const = 0;
	virtual void setUniform(int location, const glm::vec4& value) const = 0;
	virtual void setUniform(int location, const glm::mat4x4& value) const = 0;
};

