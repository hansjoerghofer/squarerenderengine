#pragma once

#include "Common/Math3D.h"

#include <any>
#include <string>

enum class UniformType : unsigned int
{
	Invalid, Int, Float, Vec4, Mat4, Texture2D, Cubemap
};

struct UniformValue
{
	UniformType type;
	std::any value;

	// Type from value
	UniformValue() : type(UniformType::Invalid) {}
	UniformValue(int v) : value(v), type(UniformType::Int) {}
	UniformValue(float v) : value(v), type(UniformType::Float) {}
	UniformValue(const glm::vec4& v) : value(v), type(UniformType::Vec4) {}
	UniformValue(const glm::mat4& v) : value(v), type(UniformType::Mat4) {}

	// Default from type
	UniformValue(UniformType t)
		: type(t)
	{
		switch (t)
		{
		case UniformType::Int:		value = int(0);			break;
		case UniformType::Float:		value = float(0.f);		break;
		case UniformType::Vec4:		value = glm::vec4(0.f,0.f,0.f,1.f); break;
		case UniformType::Mat4:		value = glm::mat4(1.f); break;
		}
	}

	template<typename T>
	T as() const
	{
		return std::any_cast<T>(value);
	}
};

struct UniformMetaInfo
{
	std::string name;
	UniformType type;

	UniformValue defaultValue;
	UniformValue minValue;
	UniformValue maxValue;
};