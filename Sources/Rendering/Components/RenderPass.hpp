#pragma once
#include <memory>
#include <vulkan/vulkan.h>

class Device;
class Swapchain;
class Subpass;
class Framebuffer;

/**
 * \brief 渲染通道类
 */
class RenderPass
{
    /**
     * \brief 设备
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 交换链
     */
    std::shared_ptr<Swapchain> m_swapchain;
    /**
     * \brief 是否已清理
     */
    bool m_cleaned = false;

public:
    /**
     * \brief vulkan 渲染通道
     */
    VkRenderPass vkRenderPass;
    /**
     * \brief 是否启用多重采样
     */
    bool enableMSAA = false;
    /**
     * \brief 是否启用深度
     */
    bool enableDepth = false;
    /**
     * \brief 所有子通道信息
     */
    std::shared_ptr<Subpass> subpass;

    /**
     * \brief 构造函数
     * \param device 设备
     * \param inSubpass 所有子通道信息
     */
    RenderPass(const std::shared_ptr<Device>& device, const std::shared_ptr<Subpass>& inSubpass);

    /**
     * \brief 析构函数
     */
    ~RenderPass();

    /**
     * \brief 清理资源
     */
    void Cleanup();

    /**
     * \brief 开始渲染通道
     * \param vkCommandBuffer 命令缓冲
     * \param framebuffer 帧缓冲
     * \param vkExtent2D 渲染区域大小
     * \param clearColorValue 清除颜色
     * \param clearDepthStencilValue 清除深度模板
     */
    void BeginRenderPass(VkCommandBuffer vkCommandBuffer, std::shared_ptr<Framebuffer> framebuffer, VkExtent2D vkExtent2D,
                         VkClearColorValue clearColorValue, VkClearDepthStencilValue clearDepthStencilValue);
};
