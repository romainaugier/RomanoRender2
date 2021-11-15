#pragma once

#include <chrono>
#include <unordered_map>
#include <string>

#include "decl.h"

// Very simple profiler to get timings of some actions at runtime to help finding hotspots in the application

struct Profiler
{
	std::unordered_map<std::string, float> times;

	FORCEINLINE auto Start() const noexcept
	{
		return std::chrono::system_clock::now();
	}

	FORCEINLINE auto End() const noexcept
	{
		return std::chrono::system_clock::now();
	}

	FORCEINLINE void Time(std::string name,
						  std::chrono::time_point<std::chrono::system_clock>& start,
						  std::chrono::time_point<std::chrono::system_clock>& end) noexcept
	{
		float time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		if (times.find(name) != times.end()) times[name] = time;
		else times.emplace(name, time);
	}
};