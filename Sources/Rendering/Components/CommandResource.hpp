#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class Device;

/**
 * \brief 命令资源类
 */
class CommandResource
{
    /**
     * \brief 设备指针
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 是否已经清理
     */
    bool m_cleaned = false;

    /**
     * \brief 一次性使用的 vkCommandBuffer
     */
    static VkCommandBuffer m_singleTimeCommandBuffer;

public:
    /**
     * \brief vulkan 命令池
     */
    static VkCommandPool graphicsVkCommandPool;
    /**
     * \brief vulkan 命令缓冲区
     */
    std::vector<VkCommandBuffer> vkCommandBuffers;

    /**
     * \brief 构造函数
     * \param device 设备指针
     * \param size 命令缓冲区数量
     */
    CommandResource(const std::shared_ptr<Device>& device, uint32_t size);

    /**
     * \brief 析构函数
     */
    ~CommandResource();

    /**
     * \brief 创建图形命令池
     * \param device 设备指针
     */
    static void CreateGraphicsCommandPool(const std::shared_ptr<Device>& device);

    /**
     * \brief 清理图形命令池
     * \param device 设备指针
     */
    static void CleanupGraphicsCommandPool(const std::shared_ptr<Device>& device);

    /**
     * \brief 开始一次性命令缓冲区
     * \param device 设备指针
     * \return
     */
    static VkCommandBuffer BeginSingleTimeGraphicsCommands(const std::shared_ptr<Device>& device);

    /**
     * \brief 结束一次性命令缓冲区
     * \param device 设备指针
     */
    static void EndSingleTimeGraphicsCommands(const std::shared_ptr<Device>& device);

    /**
     * \brief 清理资源
     */
    void Cleanup();
};
