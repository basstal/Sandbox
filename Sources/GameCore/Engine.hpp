#pragma once

class Timer;

/**
 * \brief 引擎类
 */
class Engine
{
public:
    /**
     * \brief 初始化引擎
     */
    void Initialize();

    /**
     * \brief 主循环
     */
    void MainLoop();

    /**
     * \brief 关闭引擎，清理资源
     */
    void Cleanup();
};
