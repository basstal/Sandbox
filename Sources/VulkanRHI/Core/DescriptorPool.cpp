#include "pch.hpp"

#include "DescriptorPool.hpp"

#include "CommandPool.hpp"
#include "Device.hpp"
#include "FileSystem/Logger.hpp"

Sandbox::DescriptorPool::DescriptorPool(const std::shared_ptr<Device>& device, uint32_t descriptorCount)
{
    m_device = device;

    std::vector<VkDescriptorPoolSize> poolSizes{};
    poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount});
    poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorCount});
    poolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount});
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = descriptorCount;
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    if (vkCreateDescriptorPool(m_device->vkDevice, &poolInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create descriptor pool!");
    }
}

Sandbox::DescriptorPool::~DescriptorPool() { Cleanup(); }

void Sandbox::DescriptorPool::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyDescriptorPool(m_device->vkDevice, vkDescriptorPool, nullptr);
    m_cleaned = true;
}
