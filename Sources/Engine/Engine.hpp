#pragma once
#include <memory>
#include <memory>
#include <vector>
#include <GLFW/glfw3.h>


namespace Sandbox
{
    class Window;
    class GlfwCallbackBridge;
    struct Models;
    struct MVPUboObjects;
    class Renderer;
    class Editor;
    class Camera;
    class Timer;
    class GameObject;

    class Engine
    {
    public:
        std::shared_ptr<Renderer> renderer;
        std::shared_ptr<Editor> editor;
        std::shared_ptr<Timer> timer;
        std::vector<std::shared_ptr<MVPUboObjects>> mvpObjects;

        std::vector<std::shared_ptr<GameObject>> gameObjects;

        std::vector<std::shared_ptr<Models>> models;
        std::shared_ptr<Window> window;

        bool pause = false;
        /**
         * \brief 初始化引擎
         */
        void Prepare();

        std::shared_ptr<Sandbox::MVPUboObjects> PrepareUniformBuffers(const std::shared_ptr<Models>& inModels);

        /**
         * \brief 主循环
         */
        void MainLoop();

        void Pause(GLFWwindow* window, int key, int scancode, int action, int mods);

        /**
         * \brief 关闭引擎，清理资源
         */
        void Cleanup();
    };
}
