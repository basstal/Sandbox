#include "pch.hpp"

#include "Buffer.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"

Sandbox::Buffer::Buffer(const std::shared_ptr<Device>& device, VkDeviceSize inSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    m_device = device;
    size     = inSize;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = inSize;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags       = {};

    if (vkCreateBuffer(device->vkDevice, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create buffer!");
    }
    LOGI_OLD("{}\n{}", PtrToHexString(vkBuffer), GetCallStack())

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->vkDevice, vkBuffer, &memRequirements);
    m_isNonCoherent = (properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0;
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = device->FindMemoryType(device->vkPhysicalDevice, memRequirements.memoryTypeBits, properties);
    // NOTE:be careful https://docs.vulkan.org/tutorial/latest/04_Vertex_buffers/02_Staging_buffer.html#_conclusion
    if (vkAllocateMemory(device->vkDevice, &allocInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate buffer memory!");
    }
    LOGI_OLD("{}\n{}", PtrToHexString(vkDeviceMemory), GetCallStack())

    vkBindBufferMemory(device->vkDevice, vkBuffer, vkDeviceMemory, 0);
}

Sandbox::Buffer::~Buffer() { Cleanup(); }

void Sandbox::Buffer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    LOGI("VulkanRHI", "Cleanup Buffer {}\n{}", PtrToHexString(vkBuffer), GetCallStack())
    if (m_mappedData)
    {
        vkUnmapMemory(m_device->vkDevice, vkDeviceMemory);  // 不是必须的？
    }
    vkFreeMemory(m_device->vkDevice, vkDeviceMemory, nullptr);
    vkDestroyBuffer(m_device->vkDevice, vkBuffer, nullptr);
    m_cleaned = true;
}

void Sandbox::Buffer::UpdateOffsetSize(const void* inData, size_t inOffset, size_t inSize)
{
    if (m_mappedData == nullptr)
    {
        vkMapMemory(m_device->vkDevice, vkDeviceMemory, 0, size, 0, &m_mappedData);
    }
    memcpy(static_cast<char*>(m_mappedData) + inOffset, inData, inSize);
    if (m_isNonCoherent)
    {
        VkMappedMemoryRange mappedRange{};
        mappedRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = vkDeviceMemory;
        mappedRange.offset = inOffset;
        mappedRange.size   = inSize;
        vkFlushMappedMemoryRanges(m_device->vkDevice, 1, &mappedRange);
    }
}

void Sandbox::Buffer::Update(const void* inData) { UpdateOffsetSize(inData, 0, size);}
