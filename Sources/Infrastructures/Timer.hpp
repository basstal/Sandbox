#pragma once

#include <chrono>

class Timer
{
private:
	std::chrono::high_resolution_clock::time_point m_lastTime;

public:
	Timer()
	{
		// 初始化为当前时间
		m_lastTime = std::chrono::high_resolution_clock::now();
	}

	float GetDeltaTime()
	{
		// 获取当前时间
		auto currentTime = std::chrono::high_resolution_clock::now();

		// 计算两次调用之间的时间差（deltaTime）
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();

		// 更新lastTime为当前时间
		m_lastTime = currentTime;

		return deltaTime;
	}
};
