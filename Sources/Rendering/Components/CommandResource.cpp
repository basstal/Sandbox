#include "CommandResource.hpp"
#include "Swapchain.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Base/Device.hpp"

VkCommandPool CommandResource::graphicsVkCommandPool = VK_NULL_HANDLE;
VkCommandBuffer CommandResource::m_singleTimeCommandBuffer = VK_NULL_HANDLE;

CommandResource::CommandResource(const std::shared_ptr<Device>& device, uint32_t size)
{
    m_device = device;
    vkCommandBuffers.resize(size);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = graphicsVkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());
    if (vkAllocateCommandBuffers(m_device->vkDevice, &allocInfo, vkCommandBuffers.data()) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate command buffers!");
    }
}

CommandResource::~CommandResource()
{
    Cleanup();
}


void CommandResource::CreateGraphicsCommandPool(const std::shared_ptr<Device>& device)
{
    QueueFamilyIndices queueFamilyIndices = device->queueFamilies;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = *queueFamilyIndices.graphicsFamily;
    if (vkCreateCommandPool(device->vkDevice, &poolInfo, nullptr, &graphicsVkCommandPool) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create command pool!");
    }
}

void CommandResource::CleanupGraphicsCommandPool(const std::shared_ptr<Device>& device)
{
    if (m_singleTimeCommandBuffer != VK_NULL_HANDLE)
    {
        vkFreeCommandBuffers(device->vkDevice, graphicsVkCommandPool, 1, &m_singleTimeCommandBuffer);
    }
    vkDestroyCommandPool(device->vkDevice, graphicsVkCommandPool, nullptr);
}

VkCommandBuffer CommandResource::BeginSingleTimeGraphicsCommands(const std::shared_ptr<Device>& device)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = graphicsVkCommandPool;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device->vkDevice, &allocInfo, &m_singleTimeCommandBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(m_singleTimeCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        Logger::Fatal("failed to begin recording command buffer!");
    }
    return m_singleTimeCommandBuffer;
}

void CommandResource::EndSingleTimeGraphicsCommands(const std::shared_ptr<Device>& device)
{
    if (vkEndCommandBuffer(m_singleTimeCommandBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_singleTimeCommandBuffer;

    if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        Logger::Fatal("failed to submit draw command buffer!");
    }
    if (vkQueueWaitIdle(device->graphicsQueue) != VK_SUCCESS)
    {
        Logger::Fatal("failed to wait for queue idle!");
    }
    vkFreeCommandBuffers(device->vkDevice, graphicsVkCommandPool, 1, &m_singleTimeCommandBuffer);
    m_singleTimeCommandBuffer = VK_NULL_HANDLE;
}


void CommandResource::Cleanup()
{
    if (!m_cleaned)
    {
        for (auto& commandBuffer : vkCommandBuffers)
        {
            vkFreeCommandBuffers(m_device->vkDevice, graphicsVkCommandPool, 1, &commandBuffer);
        }
        m_cleaned = true;
    }
}
