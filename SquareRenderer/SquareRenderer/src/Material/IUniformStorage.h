#pragma once

#include <glm/glm.hpp>

#include <string>

class IUniformStorage
{
public:
	virtual bool setUniform(const std::string& name, int value) = 0;
	virtual bool setUniform(const std::string& name, float value) = 0;
	virtual bool setUniform(const std::string& name, const glm::vec4& value) = 0;
	virtual bool setUniform(const std::string& name, const glm::mat4x4& value) = 0;
};