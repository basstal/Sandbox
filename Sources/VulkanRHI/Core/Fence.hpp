#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;

    class Fence
    {
    public:
        Fence(const std::shared_ptr<Device>& device);

        ~Fence();

        void Cleanup();

        void WaitForFence();

        void Reset();

        VkFence vkFence;

    private:
        std::shared_ptr<Device> m_device;
        bool m_cleaned = false;
    };
}
