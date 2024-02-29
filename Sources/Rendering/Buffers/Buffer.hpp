#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

class Device;

/**
 * \brief 缓冲区类
 */
class Buffer
{
    /**
     * \brief 是否已经清理
     */
    bool m_cleaned = false;
    /**
     * \brief 设备指针
     */
    std::shared_ptr<Device> m_device;

    /**
     * \brief 大小
     */
    VkDeviceSize m_size;

public:
    /**
     * \brief vulkan 缓冲区
     */
    VkBuffer vkBuffer;
    /**
     * \brief vulkan 设备内存
     */
    VkDeviceMemory vkDeviceMemory;

    /**
     * \brief 构造函数
     * \param device 设备
     * \param size 大小
     * \param usage 用途
     * \param properties 属性
     */
    Buffer(std::shared_ptr<Device> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    /**
     * \brief 析构函数
     */
    ~Buffer();

    /**
     * \brief 清理资源
     */
    void Cleanup();

    /**
     * \brief 拷贝数据到缓冲区
     * \param inData 数据指针
     */
    void AssignData(const void* inData);

    /**
     * \brief 拷贝缓冲区
     * \param srcBuffer 源缓冲区
     * \param dstBuffer 目标缓冲区
     * \param size 大小
     */
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};
