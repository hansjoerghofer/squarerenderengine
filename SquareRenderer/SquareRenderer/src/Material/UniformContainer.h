#pragma once

#include <glm/glm.hpp>

#include <string>
#include <variant>
#include <optional>
#include <map>

class UniformContainer
{
public:

	virtual ~UniformContainer() = 0 {};

	template<typename T>
	void setParameter(const std::string& name, const T& value);

	template<typename T>
	std::optional<T> parameter(const std::string& name) const;

protected:

	typedef std::variant<
		int,
		float,
		glm::vec4,
		glm::mat4x4
	> UniformValue;

	std::map<std::string, UniformValue> m_parameters;
};

template<typename T>
inline void UniformContainer::setParameter(const std::string& name, const T& value)
{
	m_parameters[name] = UniformValue(value);
}

template<typename T>
inline std::optional<T> UniformContainer::parameter(const std::string& name) const
{
	const auto& found = m_parameters.find(name);
	if (found != m_parameters.end())
	{
		try
		{
			return std::get<T>(found->second);
		}
		catch (const std::bad_variant_access&) {}
	}

	return std::nullopt;
}
