#include "pch.hpp"

#include "GlfwCallbackBridge.hpp"

#include "FileSystem/Logger.hpp"
#include "Misc/Ray.hpp"
#include "Platform/Window.hpp"

static std::weak_ptr<Sandbox::GlfwCallbackBridge> glfwInputBridgeWeakPtr;


static void WindowFocusCallback(GLFWwindow* window, int focused)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onWindowFocus.Trigger(window, focused);
    }
}

static void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onWindowPosition.Trigger(window, xpos, ypos);
    }
}

static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onWindowSize.Trigger(window, width, height);
    }
}

static void CursorEnterCallback(GLFWwindow* window, int entered)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onCursorEnter.Trigger(window, entered);
    }
}

static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onCursorPosition.Trigger(window, xpos, ypos);
    }
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onMouseButton.Trigger(window, button, action, mods);
    }
}


static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onScroll.Trigger(window, xoffset, yoffset);
    }
}


static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onKey.Trigger(window, key, scancode, action, mods);
        // LOGD_OLD("key {}, scancode {}, action {}, mods {}", std::to_string(key), std::to_string(scancode), std::to_string(action), std::to_string(mods))
    }
}

static void CharCallback(GLFWwindow* window, unsigned int c)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onChar.Trigger(window, c);
    }
}

static void MonitorCallback(GLFWmonitor* monitor, int event)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onMonitor.Trigger(monitor, event);
    }
}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto glfwInputBridge = glfwInputBridgeWeakPtr.lock();
    if (glfwInputBridge)
    {
        glfwInputBridge->onFramebufferSize.Trigger(window, width, height);
    }
}

static void ErrorCallback(int error, const char* description) { LOGE_OLD("code {}\n{}", std::to_string(error), description) }

void Sandbox::GlfwCallbackBridge::BindCallbacks(GLFWwindow* window)
{
    glfwInputBridgeWeakPtr = weak_from_this();
    glfwSetWindowPosCallback(window, WindowPositionCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetWindowFocusCallback(window, WindowFocusCallback);
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetMonitorCallback(MonitorCallback);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
}

void Sandbox::GlfwCallbackBridge::InstallImGuiCallback()
{
    if (m_imGuiCallbackInstalled)
    {
        return;
    }
    m_imGuiCallbackInstalled = true;
    m_handleWindowFocus      = onWindowFocus.Bind(ImGui_ImplGlfw_WindowFocusCallback);
    m_handleCursorEnter      = onCursorEnter.Bind(ImGui_ImplGlfw_CursorEnterCallback);
    m_handleCursorPosition   = onCursorPosition.Bind(ImGui_ImplGlfw_CursorPosCallback);
    m_handleMouseButton      = onMouseButton.Bind(ImGui_ImplGlfw_MouseButtonCallback);
    m_handleScroll           = onScroll.Bind(ImGui_ImplGlfw_ScrollCallback);
    m_handleKey              = onKey.Bind(ImGui_ImplGlfw_KeyCallback);
    m_handleChar             = onChar.Bind(ImGui_ImplGlfw_CharCallback);
    m_handleMonitor          = onMonitor.Bind(ImGui_ImplGlfw_MonitorCallback);
}


void Sandbox::GlfwCallbackBridge::UninstallImGuiCallback()
{
    if (!m_imGuiCallbackInstalled)
    {
        return;
    }
    m_imGuiCallbackInstalled = false;
    onWindowFocus.Unbind(m_handleWindowFocus);
    onCursorEnter.Unbind(m_handleCursorEnter);
    onCursorPosition.Unbind(m_handleCursorPosition);
    onMouseButton.Unbind(m_handleMouseButton);
    onScroll.Unbind(m_handleScroll);
    onKey.Unbind(m_handleKey);
    onChar.Unbind(m_handleChar);
    onMonitor.Unbind(m_handleMonitor);
}


Sandbox::Ray Sandbox::CursorPositionToWorldRay(GLFWwindow* window, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    ImGuiIO&     io          = ImGui::GetIO();
    ImGuiWindow* imGuiWindow = ImGui::GetCurrentWindow();
    if (imGuiWindow == nullptr)
    {
        return Sandbox::Ray();
    }
    float mouseX = io.MousePos.x, mouseY = io.MousePos.y;
    // glfwGetCursorPos(window, &mouseX, &mouseY);
    float windowWidth = imGuiWindow->Size.x, windowHeight = imGuiWindow->Size.y;
    // glfwGetWindowSize(window, &windowWidth, &windowHeight);
    float relativeX = mouseX - imGuiWindow->Pos.x, relativeY = mouseY - imGuiWindow->Pos.y;
    LOGD_OLD("mouseX {}, mouseY {}, relativeX {}, relativeY {}", std::to_string(mouseX), std::to_string(mouseY), std::to_string(relativeX), std::to_string(relativeY))
    float x = (2.0f * static_cast<float>(relativeX)) / static_cast<float>(windowWidth) - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / static_cast<float>(windowHeight);
    // float y = (2.0f * static_cast<float>(relativeY)) / static_cast<float>(windowHeight) - 1.0f;
    float     z      = 1.0f;  // 对于射线来说，Z分量通常设置为1
    glm::vec3 rayNds = glm::vec3(x, y, z);

    glm::vec4 rayClip   = glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f);
    glm::vec4 rayEye    = glm::inverse(projectionMatrix) * rayClip;
    rayEye              = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld  = glm::inverse(viewMatrix) * rayEye;
    auto      direction = glm::normalize(glm::vec3(rayWorld));
    auto      origin    = glm::vec3(glm::inverse(viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    return Sandbox::Ray(origin, direction);
}
