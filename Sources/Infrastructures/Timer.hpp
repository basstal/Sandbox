#pragma once

#include <chrono>

/**
 * \brief 计时器
 */
class Timer
{
    /**
     * \brief 上一次计时
     */
    std::chrono::high_resolution_clock::time_point m_lastTime;

    /**
     * \brief 上一次间隔计时
     */
    std::chrono::high_resolution_clock::time_point m_lastIntervalTime;

    /**
     * \brief 每帧最短间隔时间
     */
    std::chrono::nanoseconds m_interval;

    /**
     * \brief 上一帧耗时
     */
    float m_deltaTime;

public:
    Timer();

    /**
     * \brief 获得上一帧耗时
     * \return 上一帧耗时
     */
    float GetDeltaTime();

    /**
     * \brief 设置每帧最短间隔时间
     * \param fpsLimits 每秒帧数
     */
    void SetInterval(int32_t fpsLimits);

    /**
     * \brief 结束一帧
     */
    void EndFrame();

    /**
     * \brief 是否应该进行下一帧
     * \return 是否应该进行下一帧
     */
    bool ShouldTickFrame();
};
