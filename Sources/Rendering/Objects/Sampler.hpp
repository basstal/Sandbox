#pragma once
#include <memory>
#include <vulkan/vulkan.h>

class Device;

class Sampler
{
    /**
     * \brief 是否已清理
     */
    bool m_cleaned = false;
    /**
     * \brief vulkan 设备对象
     */
    std::shared_ptr<Device> m_device;

public:
    /**
     * \brief vulkan 采样器对象
     */
    VkSampler vkSampler;
    /**
     * \brief 构造函数
     * \param device 设备
     * \param samplerAddressMode 采样器地址模式
     */
    Sampler(const std::shared_ptr<Device>& device, VkSamplerAddressMode samplerAddressMode);

    /**
     * \brief 析构函数
     */
    ~Sampler();

    /**
     * \brief 清理资源
     */
    void Cleanup();
};
