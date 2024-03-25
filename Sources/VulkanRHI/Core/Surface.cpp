#include "pch.hpp"

#include "Surface.hpp"

#include "Instance.hpp"
#include "FileSystem/Logger.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"

Sandbox::Surface::Surface(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Window>& inWindow)
{
    m_instance = instance;
    window = inWindow;
    inWindow->callbackBridge->onFramebufferSize.BindMember<Surface, &Surface::OnFramebufferSize>(this);

    if (glfwCreateWindowSurface(instance->vkInstance, window->glfwWindow, nullptr, &vkSurfaceKhr) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create window surface!");
    }
}

Sandbox::Surface::~Surface()
{
    Cleanup();
}


void Sandbox::Surface::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroySurfaceKHR(m_instance->vkInstance, vkSurfaceKhr, nullptr);
    m_cleaned = true;
}

void Sandbox::Surface::OnFramebufferSize(GLFWwindow* inWindow, int width, int height)
{
    framebufferResized = true;
}
