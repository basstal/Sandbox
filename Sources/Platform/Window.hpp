#pragma once
#include <GLFW/glfw3.h>
#include <memory>

#include "Generated/Window.rfkh.h"
#include "Serialization/ISerializable.hpp"

namespace Sandbox NAMESPACE()
{
    class GlfwCallbackBridge;

    class CLASS() Window : public std::enable_shared_from_this<Window>, public ISerializable<Window>
    {
    public:
        void Prepare();

        void Cleanup();

        GLFWwindow* glfwWindow;
        // clang-format off
    /**
     * \brief 是否是窗口模式
     */
    FIELD() bool isWindow = true;
    /**
     * \brief 窗口位置 X
     */
    FIELD() int positionX = 0;
    /**
     * \brief 窗口位置 Y
     */
    FIELD() int positionY = 0;
    /**
     * \brief 窗口宽度
     */
    FIELD() int width = 1920;
    /**
     * \brief 窗口高度
     */
    FIELD() int height = 1080;
        // clang-format on

        // std::shared_ptr<WindowSerializedProperties> property;
        std::shared_ptr<GlfwCallbackBridge> callbackBridge;

    private:
        bool m_cleaned = false;
        Sandbox_Window_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Window_GENERATED
