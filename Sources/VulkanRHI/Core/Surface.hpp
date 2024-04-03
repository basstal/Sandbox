#pragma once
#include "VulkanRHI/Common/Macros.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Sandbox {
class Instance;
class Window;

class Surface
{
  public:
    Surface(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Window>& inWindow);

    ~Surface();

    DISABLE_COPY_AND_MOVE(Surface)

    void Cleanup();

    void OnFramebufferSize(GLFWwindow* window, int width, int height);

    VkSurfaceKHR vkSurfaceKhr;

    // TODO: replace this??
    std::shared_ptr<Window> window;

    bool framebufferResized = false;

  private:
    bool m_cleaned = false;
    std::shared_ptr<Instance> m_instance;
};
} // namespace Sandbox
