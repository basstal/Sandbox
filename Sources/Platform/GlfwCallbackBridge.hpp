#pragma once
#include "GLFW/glfw3.h"
#include <glm/mat4x4.hpp>
#include "Misc/Event.hpp"

namespace Sandbox
{
    class Ray;

    class GlfwCallbackBridge : public std::enable_shared_from_this<GlfwCallbackBridge>
    {
    public:
        Event<GLFWwindow*, int> onWindowFocus;
        Event<GLFWwindow*, int> onCursorEnter;
        Event<GLFWwindow*, double, double> onCursorPosition;
        Event<GLFWwindow*, int, int, int> onMouseButton;
        Event<GLFWwindow*, double, double> onScroll;
        Event<GLFWwindow*, int, int, int, int> onKey;
        Event<GLFWwindow*, unsigned int> onChar;
        Event<GLFWmonitor*, int> onMonitor;
        Event<GLFWwindow*, int, int> onWindowPosition;
        Event<GLFWwindow*, int, int> onWindowSize;
        Event<GLFWwindow*, int, int> onFramebufferSize;


        void BindCallbacks(GLFWwindow* window);

        void InstallImGuiCallback();

        void UninstallImGuiCallback();

    private:
        bool m_imGuiCallbackInstalled = false;
        DelegateHandle m_handleWindowFocus;
        DelegateHandle m_handleCursorEnter;
        DelegateHandle m_handleCursorPosition;
        DelegateHandle m_handleMouseButton;
        DelegateHandle m_handleScroll;
        DelegateHandle m_handleKey;
        DelegateHandle m_handleChar;
        DelegateHandle m_handleMonitor;
        DelegateHandle m_handleWindowPosition;
        DelegateHandle m_handleWindowSize;
    };

    // TODO:这个计算可能有问题
    Ray CursorPositionToWorldRay(GLFWwindow* window, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
}
