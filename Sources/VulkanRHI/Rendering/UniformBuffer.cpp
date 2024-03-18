#include "UniformBuffer.hpp"

#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/Device.hpp"

Sandbox::UniformBuffer::UniformBuffer(const std::shared_ptr<Device>& device, VkDeviceSize inSize)
{
    m_device = device;
    buffer = std::make_shared<Buffer>(device, inSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

VkDescriptorBufferInfo Sandbox::UniformBuffer::GetDescriptorBufferInfo(VkDeviceSize range)
{
    VkDescriptorBufferInfo vkDescriptorBufferInfo;
    vkDescriptorBufferInfo.buffer = buffer->vkBuffer;
    vkDescriptorBufferInfo.offset = 0;
    vkDescriptorBufferInfo.range = range == 0 ? buffer->size : range;
    return vkDescriptorBufferInfo;
}

Sandbox::UniformBuffer::~UniformBuffer()
{
    Cleanup();
}

void Sandbox::UniformBuffer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    buffer->Cleanup();
    m_cleaned = true;
}

void Sandbox::UniformBuffer::Update(const void* inData)
{
    buffer->Update(inData);
}
