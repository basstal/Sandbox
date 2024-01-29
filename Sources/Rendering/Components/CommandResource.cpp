#include "CommandResource.hpp"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Base/Device.hpp"


CommandResource::CommandResource(const std::shared_ptr<Device>& device)
{
	m_device = device;
	QueueFamilyIndices queueFamilyIndices = device->FindQueueFamilies();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = *queueFamilyIndices.graphicsFamily;
	if (vkCreateCommandPool(device->vkDevice, &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

CommandResource::~CommandResource()
{
	Cleanup();
}


VkCommandBuffer CommandResource::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device->vkDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

void CommandResource::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_device->graphicsQueue);
	vkFreeCommandBuffers(m_device->vkDevice, vkCommandPool, 1, &commandBuffer);
}

void CommandResource::CreateCommandBuffers()
{
	vkCommandBuffers.resize(m_device->MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());
	if (vkAllocateCommandBuffers(m_device->vkDevice, &allocInfo, vkCommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void CommandResource::Cleanup()
{
	if (!m_cleaned)
	{
		vkDestroyCommandPool(m_device->vkDevice, vkCommandPool, nullptr);
		m_cleaned = true;
	}
}
