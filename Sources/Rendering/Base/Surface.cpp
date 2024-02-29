#include "Surface.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/RendererSettings.hpp"

void WindowPosCallback(GLFWwindow* window, int x, int y)
{
    auto currentIsWindow = glfwGetWindowMonitor(window) == nullptr;
    if (currentIsWindow)
    {
        auto settings = DataBinding::Get<std::shared_ptr<RendererSettings>>("Rendering/Settings")->GetData();
        settings->persistence.windowPositionX = x;
        settings->persistence.windowPositionY = y;
    }
}

void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    const auto surface = static_cast<Surface*>(glfwGetWindowUserPointer(window));
    surface->framebufferResized = true;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    auto currentIsWindow = glfwGetWindowMonitor(window) == nullptr;
    if (currentIsWindow)
    {
        auto settings = DataBinding::Get<std::shared_ptr<RendererSettings>>("Rendering/Settings")->GetData();
        settings->persistence.width = width;
        settings->persistence.height = height;
    }
}

Surface::Surface(const VkInstance& instance)
{
    m_vkInstance = instance;
    auto settingsBinding = DataBinding::Get<std::shared_ptr<RendererSettings>>("Rendering/Settings");
    auto settings = settingsBinding->GetData();
    settingsBinding->BindMember<Surface, &Surface::ApplySettings>(this);
    // 初始化 GLFW 库
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // 获取一个指向用户的主监视器的指针
    GLFWmonitor* monitor = settings->persistence.isWindow ? nullptr : glfwGetPrimaryMonitor();
    // 获取该监视器的视频模式，特别是为了得到屏幕的分辨率。你可以使用glfwGetVideoMode来获取当前视频模式：
    const GLFWvidmode* mode = settings->persistence.isWindow ? nullptr : glfwGetVideoMode(monitor);
    int width = settings->persistence.isWindow ? (int)settings->persistence.width : mode->width;
    int height = settings->persistence.isWindow ? (int)settings->persistence.height : mode->height;
    // 创建窗口
    glfwWindow = glfwCreateWindow(width, height, settings->persistence.applicationName.c_str(), monitor, nullptr);
    if (glfwWindow == nullptr)
    {
        Logger::Fatal("failed to create window!");
    }
    glfwSetWindowPos(glfwWindow, (int)settings->persistence.windowPositionX, (int)settings->persistence.windowPositionY);
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);
    glfwSetWindowPosCallback(glfwWindow, WindowPosCallback);
    glfwSetWindowSizeCallback(glfwWindow, WindowSizeCallback);
    if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &vkSurface) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create window surface!");
    }
}

Surface::~Surface()
{
    Cleanup();
}

void Surface::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroySurfaceKHR(m_vkInstance, vkSurface, nullptr);
    glfwDestroyWindow(glfwWindow);
    m_cleaned = true;
}


void Surface::ApplySettings(std::shared_ptr<RendererSettings> inSettings)
{
    if (glfwWindow == nullptr)
    {
        return;
    }
    auto currentIsWindow = glfwGetWindowMonitor(glfwWindow) == nullptr;

    if (inSettings->persistence.isWindow != currentIsWindow)
    {
        if (inSettings->persistence.isWindow)
        {
            // 当前是全屏模式，切换到窗口模式
            glfwSetWindowMonitor(glfwWindow, nullptr,
                                 (int)inSettings->persistence.windowPositionX, (int)inSettings->persistence.windowPositionY,
                                 (int)inSettings->persistence.width, (int)inSettings->persistence.height, 0);
        }
        else
        {
            // 获取当前视频模式和主监视器
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // 切换到全屏模式
            glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }
    if (inSettings->persistence.isWindow)
    {
        int posX, posY, width, height;
        // 保存当前窗口位置和大小
        glfwGetWindowPos(glfwWindow, &posX, &posY);
        glfwGetWindowSize(glfwWindow, &width, &height);
        if (posX != inSettings->persistence.windowPositionX || posY != inSettings->persistence.windowPositionY || width != inSettings->persistence.width || height != inSettings->
            persistence.height)
        {
            glfwSetWindowPos(glfwWindow, (int)inSettings->persistence.windowPositionX, (int)inSettings->persistence.windowPositionY);
            glfwSetWindowSize(glfwWindow, (int)inSettings->persistence.width, (int)inSettings->persistence.height);
        }
    }
}
