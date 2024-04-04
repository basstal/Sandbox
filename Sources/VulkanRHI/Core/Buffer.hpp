#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Sandbox
{
    class Device;

    class Buffer
    {
    public:
        VkBuffer vkBuffer;

        VkDeviceMemory vkDeviceMemory;

        VkDeviceSize size;

        Buffer(const std::shared_ptr<Device>& device, VkDeviceSize inSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        ~Buffer();

        void Cleanup();

        void Update(const void* inData);

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned       = false;
        bool                    m_isNonCoherent = false;
        void*                   m_mappedData    = nullptr;
    };
}  // namespace Sandbox
