#pragma once

#include "Common/Logger.h"

#include <chrono>

class Timer
{
public:
	explicit Timer()
		: m_start(std::chrono::system_clock::now())
	{}

	double elapsedMs() const
	{
		const auto end = std::chrono::system_clock::now();
		const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start);

		// to milliseconds 
		return elapsed.count() * 1e-6;
	}

	double elapsed() const
	{
		return elapsedMs() * 1e-3;
	}

	void reset()
	{
		m_start = std::chrono::system_clock::now();
	}

private:
	std::chrono::system_clock::time_point m_start;
};

class ScopedTimer : Timer
{
public:
	explicit ScopedTimer(double& outElapsed)
		: Timer()
		, m_outElapsed(outElapsed)
	{
	}

	~ScopedTimer()
	{
		m_outElapsed = elapsed();
	}

private:
	double& m_outElapsed;
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

