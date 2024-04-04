#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;

    class CommandPool
    {
    public:
        CommandPool(const std::shared_ptr<Device>& device);

        ~CommandPool();

        void Cleanup();

        VkCommandPool vkCommandPool;

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
