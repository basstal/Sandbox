#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;

    class Sampler
    {
    public:
        Sampler(const std::shared_ptr<Device>& device, VkSamplerAddressMode samplerAddressMode);

        ~Sampler();

        void Cleanup();

        VkSampler vkSampler;
    private:
        std::shared_ptr<Device> m_device;
        bool m_cleaned = false;
    };
}
