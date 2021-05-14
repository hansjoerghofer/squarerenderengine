#include "Common/Logger.h"

void Logger::log(LogSeverity s, const std::string& message)
{
	try
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			if (!it->expired())
			{
				// invoke callback
				(*it->lock())(s, message);
			}
			else
			{
				// remove expired callbacks
				it = m_callbacks.erase(it);
			}
		}
	}
	catch (...) 
	{
		// what shall we do?
	}
}

LogCallbackSPtr Logger::registerCallback(LogCallback&& func)
{
	LogCallbackSPtr ptr(new LogCallback(std::move(func)));

	m_callbacks.push_back(ptr);

	return ptr;
}

Logger& Logger::getInstance()
{
	static Logger instance;
	return instance;
}

std::string Logger::severity(LogSeverity s)
{
	switch (s)
	{
	case LogSeverity::Debug: return "Debug";
	case LogSeverity::Info: return "Info";
	case LogSeverity::Warning: return "Warning";
	case LogSeverity::Error: return "Error";
	default: return "";
	}
}
