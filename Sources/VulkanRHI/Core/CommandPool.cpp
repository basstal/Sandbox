#include "pch.hpp"

#include "CommandPool.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"

Sandbox::CommandPool::CommandPool(const std::shared_ptr<Device>& device)
{
    m_device = device;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = *device->queueFamilyIndices.graphicsFamily;
    if (vkCreateCommandPool(m_device->vkDevice, &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create command pool!");
    }
}

Sandbox::CommandPool::~CommandPool() { Cleanup(); }

void Sandbox::CommandPool::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyCommandPool(m_device->vkDevice, vkCommandPool, nullptr);
    m_cleaned = true;
}
