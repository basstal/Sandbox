#pragma once

#include <chrono>

class Timer
{
private:
	std::chrono::high_resolution_clock::time_point m_lastTime;
	std::chrono::high_resolution_clock::time_point m_lastIntervalTime;
	std::chrono::nanoseconds m_interval;

public:
	float deltaTime;
	Timer();
	void SetInterval(int32_t fpsLimits);
	void EndFrame();
	bool ShouldTickFrame();
};
