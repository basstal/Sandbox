#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

class Framebuffer;
class Surface;
class Image;
class RenderPass;
class Device;
/**
 * \brief 交换链
 */
class Swapchain
{
    /**
     * \brief 选择交换链格式
     * \param availableFormats 可用格式
     * \return 交换链格式
     */
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    /**
     * \brief 选择交换链图像大小
     * \param window glfw 窗口
     * \param capabilities 交换链能力
     * \return 交换链图像大小
     */
    VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

    /**
     * \brief 选择交换链呈现模式
     * \param availablePresentModes 可用呈现模式
     * \return 交换链呈现模式
     */
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    /**
     * \brief vulkan 设备
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 交换链额外颜色图像
     */
    std::shared_ptr<Image> m_colorImage;
    /**
     * \brief 交换链深度图像
     */
    std::shared_ptr<Image> m_depthImage;

    /**
     * \brief 是否清理
     */
    bool m_cleaned = false;

public:
    /**
     * \brief 最大同时渲染的帧数
     */
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    /**
     * \brief 交换链颜色格式
     */
    static constexpr VkFormat COLOR_FORMAT = VK_FORMAT_B8G8R8A8_UNORM;
    /**
     * \brief 交换链颜色空间
     */
    static constexpr VkColorSpaceKHR COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    /**
     * \brief 交换链
     */
    VkSwapchainKHR vkSwapchain;
    /**
     * \brief 交换链图像格式
     */
    VkFormat swapchainVkFormat;
    /**
     * \brief 交换链图像大小
     */
    VkExtent2D swapchainVkExtent2D;
    /**
     * \brief 交换链图像
     */
    std::vector<VkImage> swapchainVkImages;
    /**
     * \brief 交换链图像视图
     */
    std::vector<VkImageView> swapchainVkImageViews;
    /**
     * \brief 交换链帧缓冲
     */
    std::vector<std::shared_ptr<Framebuffer>> framebuffers;

    /**
     * \brief 创建交换链
     * \param surface 表面
     * \param device 设备
     */
    Swapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device);

    /**
     * \brief 析构函数
     */
    ~Swapchain();

    /**
     * \brief 清理资源
     */
    void Cleanup();

    /**
     * \brief 创建交换链帧缓冲
     * \param renderPass 渲染通道
     */
    void CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass);

    /**
     * \brief 创建交换链
     * \param surface 表面
     * \param device 设备
     */
    void CreateSwapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device);
};
