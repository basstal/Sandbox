#include "pch.hpp"

#include "Fence.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"

Sandbox::Fence::Fence(const std::shared_ptr<Device>& device)
{
    m_device = device;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateFence(device->vkDevice, &fenceInfo, nullptr, &vkFence) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create synchronization objects for a frame!");
    }
}

Sandbox::Fence::~Fence() { Cleanup(); }

void Sandbox::Fence::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyFence(m_device->vkDevice, vkFence, nullptr);
    m_cleaned = true;
}


void Sandbox::Fence::WaitForFence() { vkWaitForFences(m_device->vkDevice, 1, &vkFence, VK_TRUE, UINT64_MAX); }

/**
 * \brief Only reset the fence if we are submitting work
 */
void Sandbox::Fence::Reset() { vkResetFences(m_device->vkDevice, 1, &vkFence); }
