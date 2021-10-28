#pragma once

#include "Common/Macros.h"

#include <string>
#include <sstream>
#include <list>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>

#pragma warning( push )
#pragma warning( disable : 4840 )

/* source: 
 * https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
 */
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	auto buf = std::make_unique<char[]>(size);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

#pragma warning( pop )

enum class LogSeverity : char
{
	Debug,
	Info,
	Warning,
	Error
};

typedef std::function<void(LogSeverity, const std::string&)> LogCallback;
typedef std::shared_ptr<LogCallback> LogCallbackSPtr;
typedef std::weak_ptr<LogCallback> LogCallbackWPtr;

DECLARE_PTRS(Logger);

class Logger
{
public:

	void log(LogSeverity s, const std::string& message);

	LogCallbackSPtr registerCallback(LogCallback&& func);

	static Logger& getInstance();

	static std::string severity(LogSeverity s);

	template<typename ... Args>
#ifdef _DEBUG
	static void Debug(const std::string& message, Args ... args)
	{
		Logger::getInstance().log(
			LogSeverity::Debug, string_format(message, args ...));
	}
#else
	static void Debug(const std::string&, Args ...) {}
#endif

	template<typename ... Args>
	static void Info(const std::string& message, Args ... args)
	{
		Logger::getInstance().log(
			LogSeverity::Info, string_format(message, args ...));
	}

	template<typename ... Args>
	static void Warning(const std::string& message, Args ... args)
	{
		Logger::getInstance().log(
			LogSeverity::Warning, string_format(message, args ...));
	}

	template<typename ... Args>
	static void Error(const std::string& message, Args ... args)
	{
		Logger::getInstance().log(
			LogSeverity::Error, string_format(message, args ...));
	}

private:

	Logger() = default;
	~Logger() = default;

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	std::list<LogCallbackWPtr> m_callbacks;
};
