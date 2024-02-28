#include "Buffer.hpp"
#include "Rendering/Base/Device.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"
#include "Rendering/Base/Properties.hpp"


Buffer::Buffer(const std::shared_ptr<Device> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    m_device = device;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device->vkDevice, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->vkDevice, vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FIND_MEMORY_TYPE(device->vkPhysicalDevice,
                                                 memRequirements.memoryTypeBits, properties);
    // NOTE:be careful https://docs.vulkan.org/tutorial/latest/04_Vertex_buffers/02_Staging_buffer.html#_conclusion
    if (vkAllocateMemory(device->vkDevice, &allocInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate buffer memory!");
    }
    vkBindBufferMemory(device->vkDevice, vkBuffer, vkDeviceMemory, 0);
}

Buffer::~Buffer()
{
    // Cleanup();
}

void Buffer::Cleanup()
{
    if (!m_cleaned)
    {
        // TODO:recover
        // vmaDestroyBuffer(allocator, vkBuffer, allocation);
        // vkDestroyBuffer(m_device->vkDevice, vkBuffer, nullptr);
        // vkFreeMemory(m_device->vkDevice, vkDeviceMemory, nullptr);
        m_cleaned = true;
    }
}
