#pragma once
#include <chrono>

namespace Sandbox
{
    class Timer
    {
    public:
        Timer();

        /**
         * \brief 获得上一帧耗时
         * \return 上一帧耗时
         */
        float GetDeltaTime();

        // /**
        //  * \brief 设置每帧最短间隔时间
        //  * \param fpsLimits 每秒帧数
        //  */
        // void SetInterval(int32_t fpsLimits);

        /**
         * \brief 结束一帧
         */
        void EndFrame();

        void UpdateInFixed(const std::chrono::microseconds& fixedDeltaTime);
        bool BeginFixed();
        void EndFixed();

        bool UpdateInInterval(const std::chrono::microseconds& interval);

    private:
        /**
         * \brief 上一次计时
         */
        std::chrono::high_resolution_clock::time_point m_lastTime;

        /**
         * \brief 上一次间隔计时
         */
        std::chrono::high_resolution_clock::time_point m_lastIntervalTime;

        std::chrono::high_resolution_clock::time_point m_lastFixedTime;
        // /**
        //  * \brief 每帧最短间隔时间
        //  */
        // std::chrono::milliseconds m_interval;


        std::chrono::microseconds m_fixedLag;
        std::chrono::microseconds m_fixedDeltaTime;

        /**
         * \brief 上一帧耗时
         */
        float m_deltaTime;
    };
}
