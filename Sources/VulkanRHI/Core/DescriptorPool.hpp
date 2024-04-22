#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;

    class DescriptorPool
    {
    public:
        DescriptorPool(const std::shared_ptr<Device>& device, uint32_t descriptorCount);

        ~DescriptorPool();

        void Cleanup();

        VkDescriptorPool vkDescriptorPool;

        std::shared_ptr<Device> GetDevice();

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
