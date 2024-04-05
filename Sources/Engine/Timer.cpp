#include "pch.hpp"

#include "Timer.hpp"

#include "Misc/TypeCasting.hpp"


Sandbox::Timer::Timer() : m_fixedLag(1), m_fixedDeltaTime(1)
{
    m_lastTime         = std::chrono::high_resolution_clock::now();
    m_lastIntervalTime = m_lastFixedTime = m_lastTime;
    // m_interval  = std::chrono::milliseconds(1);  // 避免除 0
    m_deltaTime = 0.0f;
}

// void Sandbox::Timer::SetInterval(int32_t fpsLimits) { m_interval = std::chrono::milliseconds(1000 / fpsLimits); }

void Sandbox::Timer::EndFrame()
{
    // 获取当前时间
    auto currentTime = std::chrono::high_resolution_clock::now();
    // 计算到上一次调用的时间差
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();
    m_lastTime  = currentTime;
}

bool Sandbox::Timer::UpdateInInterval(const std::chrono::microseconds& interval)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastIntervalTime);
    auto elapsed     = ToUInt32(std::floor(elapsedTime / interval));
    if (elapsed != 0)
    {
        m_lastIntervalTime = currentTime;
        return true;
    }
    return false;
}

void Sandbox::Timer::UpdateInFixed(const std::chrono::microseconds& fixedDeltaTime)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastFixedTime);
    m_lastFixedTime  = currentTime;
    m_fixedLag += elapsedTime;
    m_fixedDeltaTime = fixedDeltaTime;
}

bool Sandbox::Timer::BeginFixed() { return m_fixedLag >= m_fixedDeltaTime; }

void Sandbox::Timer::EndFixed() { m_fixedLag -= m_fixedDeltaTime; }


float Sandbox::Timer::GetDeltaTime()
{
    return m_deltaTime;
}
