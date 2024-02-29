#pragma once
#include <memory>

class Timer;
class Scene;

/**
 * \brief 引擎类
 */
class Engine
{
public:
    // TODO: remove this
    std::shared_ptr<Scene> scene;
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
