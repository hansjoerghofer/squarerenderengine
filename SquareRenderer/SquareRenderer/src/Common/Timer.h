#pragma once

#include "Common/Logger.h"

#include <chrono>

class Timer
{
public:
	explicit Timer()
		: m_start(std::chrono::system_clock::now())
	{}

	double elapsed() const
	{
		const auto end = std::chrono::system_clock::now();
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);

		// to seconds
		return elapsed.count() * 0.001;
	}

	void reset()
	{
		m_start = std::chrono::system_clock::now();
	}

private:
	std::chrono::system_clock::time_point m_start;
};

class TimerLog
{
public:
	explicit TimerLog()
		: m_timer()
	{}

	void logElapsed(const std::string& description)
	{
		Logger::Debug("Timer '%s' took: %.3f sec", description.c_str(), m_timer.elapsed());
	}

	void reset()
	{
		m_timer.reset();
	}

private:
	Timer m_timer;
};

class ScopedTimerLog : public TimerLog
{
public:
	ScopedTimerLog(const std::string& descriptor)
		: TimerLog()
		, m_descriptor(descriptor)
	{}

	ScopedTimerLog(std::string&& descriptor)
		: TimerLog()
		, m_descriptor(std::move(descriptor))
	{}

	~ScopedTimerLog()
	{
		logElapsed(m_descriptor);
	}

private:
	std::string m_descriptor;
};

