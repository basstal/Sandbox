#pragma once
#define GLFW_INCLUDE_VULKAN
#include <memory>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

class RendererSettings;
/**
 * \brief 包含 vulkan 的 Surface 和 GLFW 的窗口
 */
class Surface
{
    /**
     * \brief 应用配置信息
     * \param inSettings 配置信息
     */
    void ApplySettings(std::shared_ptr<RendererSettings> inSettings);

    /**
     * \brief vulkan 实例
     */
    VkInstance m_vkInstance;

    /**
     * \brief 是否已经清理
     */
    bool m_cleaned = false;

public:
    /**
     * \brief frameBuffer 大小是否被调整
     */
    bool framebufferResized = false;
    /**
     * \brief GLFW 窗口
     */
    GLFWwindow* glfwWindow;
    /**
     * \brief vulkan 的 Surface
     */
    VkSurfaceKHR vkSurface;

    Surface(const VkInstance& instance);

    ~Surface();

    /**
     * \brief 清理资源
     */
    void Cleanup();
};
