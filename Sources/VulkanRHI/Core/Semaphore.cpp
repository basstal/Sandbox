#include "pch.hpp"

#include "Semaphore.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"

Sandbox::Semaphore::Semaphore(const std::shared_ptr<Device>& device)
{
    m_device = device;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(m_device->vkDevice, &semaphoreInfo, nullptr, &vkSemaphore) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create synchronization objects for a frame!");
    }
}

Sandbox::Semaphore::~Semaphore() { Cleanup(); }

void Sandbox::Semaphore::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroySemaphore(m_device->vkDevice, vkSemaphore, nullptr);
    m_cleaned = true;
}
