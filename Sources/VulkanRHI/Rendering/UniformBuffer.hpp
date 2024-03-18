#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"


namespace Sandbox
{
    class Buffer;
    class Device;

    class UniformBuffer
    {
    public:
        UniformBuffer(const std::shared_ptr<Device>& device, VkDeviceSize inSize);

        VkDescriptorBufferInfo GetDescriptorBufferInfo(VkDeviceSize range = 0);

        ~UniformBuffer();

        std::shared_ptr<Buffer> buffer;

        void Cleanup();

        void Update(const void* inData);
    private:
        std::shared_ptr<Device> m_device;
        bool m_cleaned = false;
    };
}
