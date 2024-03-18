#pragma once
#include <memory>
#include <boost/hana.hpp>

#include "GLFW/glfw3.h"


namespace Sandbox
{
    class GlfwCallbackBridge;

    struct WindowSerializedProperties
    {
        /**
     * \brief 是否是窗口模式
     */
        bool isWindow = true;
        /**
         * \brief 窗口位置 X
         */
        int windowPositionX = 0;
        /**
         * \brief 窗口位置 Y
         */
        int windowPositionY = 0;
        /**
         * \brief 窗口宽度
         */
        int width = 1920;
        /**
         * \brief 窗口高度
         */
        int height = 1080;
    };

    class Window : public std::enable_shared_from_this<Window>
    {
    public:
        void Prepare();

        void Cleanup();

        GLFWwindow* glfwWindow;

        std::shared_ptr<WindowSerializedProperties> property;
        std::shared_ptr<GlfwCallbackBridge> callbackBridge;

    private:
        bool m_cleaned = false;
    };
}

BOOST_HANA_ADAPT_STRUCT(Sandbox::WindowSerializedProperties,
                        isWindow,
                        windowPositionX,
                        windowPositionY,
                        width,
                        height);
