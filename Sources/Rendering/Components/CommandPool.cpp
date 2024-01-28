#include "CommandPool.hpp"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Base/Device.hpp"


CommandPool::CommandPool(const std::shared_ptr<Device>& device)
{
	QueueFamilyIndices queueFamilyIndices = device->FindQueueFamilies();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	if (vkCreateCommandPool(device->vkDevice, &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}
CommandPool::~CommandPool()
{
}
