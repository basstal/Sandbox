#pragma once
#include "Base/Device.hpp"

class CommandPool
{
private:
	std::shared_ptr<Device> m_device;
public:
	VkCommandPool vkCommandPool;
	CommandPool(const std::shared_ptr<Device>& device);
	~CommandPool();
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
};
