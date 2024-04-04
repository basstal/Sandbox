#include "pch.hpp"

#include "Window.hpp"

#include "FileSystem/Directory.hpp"
#include "FileSystem/Logger.hpp"
#include "Generated/Window.rfks.h"
#include "Platform/GlfwCallbackBridge.hpp"

#ifdef _WIN64
constexpr int32_t MIN_POSITION_Y = 40;
#else
constexpr int32_t MIN_POSITION_Y = 0;
#endif

static std::weak_ptr<Sandbox::Window> windowWeakPtr;
static Sandbox::File                  cacheFile = Sandbox::Directory::GetLibraryDirectory().GetFile("WindowConfigCache.yaml");

static void WindowPosCallback(GLFWwindow* glfwWindow, int x, int y)
{
    auto window = windowWeakPtr.lock();
    if (window && glfwGetWindowMonitor(glfwWindow) == nullptr && window->glfwWindow == glfwWindow)  // 仅处理当前 Window 创建的 glfwWindow
    {
        window->positionX = x;
        window->positionY = std::max(y, MIN_POSITION_Y);
    }
}

static void WindowSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
    auto window = windowWeakPtr.lock();
    if (window && glfwGetWindowMonitor(glfwWindow) == nullptr && window->glfwWindow == glfwWindow)  // 仅处理当前 Window 创建的 glfwWindow
    {
        window->width  = width;
        window->height = height;
    }
}

void Sandbox::Window::Prepare()
{
    LoadFromFile(cacheFile);
    callbackBridge = std::make_shared<GlfwCallbackBridge>();
    // property = std::make_shared<WindowSerializedProperties>();
    // 初始化 GLFW 库
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // 获取一个指向用户的主监视器的指针
    GLFWmonitor* monitor = isWindow ? nullptr : glfwGetPrimaryMonitor();
    // 获取该监视器的视频模式，特别是为了得到屏幕的分辨率。你可以使用glfwGetVideoMode来获取当前视频模式：
    const GLFWvidmode* mode         = isWindow ? nullptr : glfwGetVideoMode(monitor);
    int                windowWidth  = isWindow ? this->width : mode->width;
    int                windowHeight = isWindow ? this->height : mode->height;
    // 创建窗口
    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Sandbox", monitor, nullptr);
    if (glfwWindow == nullptr || !glfwVulkanSupported())
    {
        Logger::Fatal("failed to create window!");
    }
    callbackBridge->BindCallbacks(glfwWindow);
    glfwSetWindowPos(glfwWindow, positionX, std::max(positionY, MIN_POSITION_Y));
    callbackBridge->onWindowPosition.Bind(WindowPosCallback);
    callbackBridge->onWindowSize.Bind(WindowSizeCallback);
    callbackBridge->InstallImGuiCallback();
    windowWeakPtr = weak_from_this();
    m_cleaned     = false;
}

void Sandbox::Window::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    SaveToFile(cacheFile);
    callbackBridge->UninstallImGuiCallback();
    glfwDestroyWindow(glfwWindow);
    m_cleaned = true;
}
