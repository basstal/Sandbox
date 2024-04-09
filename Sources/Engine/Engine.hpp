#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>


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
    class Scene;
    class Engine
    {
    public:
        // std::shared_ptr<Scene> scene;

        std::shared_ptr<Renderer>                   renderer;
        std::shared_ptr<Editor>                     editor;
        std::shared_ptr<Timer>                      rendererTimer;
        std::shared_ptr<Timer>                      logicTimer;
        std::vector<std::shared_ptr<MVPUboObjects>> mvpObjects;

        // std::vector<std::shared_ptr<GameObject>> gameObjects;

        std::vector<std::shared_ptr<Models>> models;
        std::shared_ptr<Window>              window;

        bool pause = false;
        bool shouldRecreateRenderer;
        /**
         * \brief 初始化引擎
         */
        void Prepare();
        // void SetScene(const std::shared_ptr<Scene>& inScene);
        // void NewScene();
        void CreateEditor();
        void CreateRenderer();

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

