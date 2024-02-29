#pragma once
#include <memory>

#include "Buffer.hpp"
#include "Rendering/Base/Device.hpp"


/**
 * \brief 统一缓冲区类
 * \tparam T 缓冲区数据类型
 */
template <typename T>
class UniformBuffer
{
    /**
     * \brief 设备指针
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 是否已清理
     */
    bool m_cleanup = false;
    /**
     * \brief vulkan 描述符缓冲区信息
     */
    VkDescriptorBufferInfo m_vkDescriptorBufferInfo = {};

public:
    /**
     * \brief 缓冲区
     */
    std::shared_ptr<Buffer> buffer;
    /**
     * \brief vulkan 写描述符集
     */
    VkWriteDescriptorSet vkWriteDescriptorSet = {};


    /**
     * \brief 构造函数
     * \param device 设备
     * \param binding 绑定
     * \param descriptorSet 描述符集
     */
    UniformBuffer(const std::shared_ptr<Device>& device, uint32_t binding, VkDescriptorSet descriptorSet);

    /**
     * \brief 析构函数
     */
    ~UniformBuffer();

    /**
     * \brief 提交数据
     * \param data 数据
     */
    void AssignData(T data);


    /**
     * \brief 清理
     */
    void Cleanup();
};

template <typename T>
UniformBuffer<T>::UniformBuffer(const std::shared_ptr<Device>& device, uint32_t binding, VkDescriptorSet descriptorSet)
{
    m_device = device;
    buffer = std::make_shared<Buffer>(device, sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


    m_vkDescriptorBufferInfo.buffer = buffer->vkBuffer;
    m_vkDescriptorBufferInfo.offset = 0;
    m_vkDescriptorBufferInfo.range = sizeof(T);


    vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vkWriteDescriptorSet.dstSet = descriptorSet;
    vkWriteDescriptorSet.dstBinding = binding;
    vkWriteDescriptorSet.dstArrayElement = 0;
    vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vkWriteDescriptorSet.descriptorCount = 1;
    vkWriteDescriptorSet.pBufferInfo = &m_vkDescriptorBufferInfo;
}

template <typename T>
UniformBuffer<T>::~UniformBuffer()
{
    Cleanup();
}

template <typename T>
void UniformBuffer<T>::AssignData(T data)
{
    void* mappedData;
    vkMapMemory(m_device->vkDevice, buffer->vkDeviceMemory, 0, sizeof(T), 0, &mappedData);
    memcpy(mappedData, &data, sizeof(T));
    vkUnmapMemory(m_device->vkDevice, buffer->vkDeviceMemory);
}

template <typename T>
void UniformBuffer<T>::Cleanup()
{
    if (!m_cleanup)
    {
        buffer->Cleanup();
        m_cleanup = true;
    }
}
