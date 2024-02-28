#include "Timer.hpp"
#include <thread>

Timer::Timer()
{
    m_lastTime = std::chrono::high_resolution_clock::now();
    m_interval = std::chrono::nanoseconds(0);
    m_deltaTime = 0.0f;
}

void Timer::SetInterval(int32_t fpsLimits)
{
    m_interval = std::chrono::nanoseconds(1000000000 / fpsLimits);
}

void Timer::EndFrame()
{
    // 获取当前时间
    auto currentTime = std::chrono::high_resolution_clock::now();
    // 计算到上一次调用的时间差
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();
    m_lastTime = currentTime;
}

bool Timer::ShouldTickFrame()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastIntervalTime);
    auto elapsed = static_cast<uint32_t>(std::floor(elapsedTime / m_interval));
    if (elapsed != 0)
    {
        m_lastIntervalTime = currentTime;
        return true;
    }
    return false;
}

float Timer::GetDeltaTime()
{
    return m_deltaTime;
}
