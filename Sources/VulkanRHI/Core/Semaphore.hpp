#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Sandbox
{
    class Device;

    class Semaphore
    {
    public:
        Semaphore(const std::shared_ptr<Device>& device);

        ~Semaphore();

        void Cleanup();

        VkSemaphore vkSemaphore;

    private:
        std::shared_ptr<Device> m_device;
        bool m_cleaned = false;
    };
}
