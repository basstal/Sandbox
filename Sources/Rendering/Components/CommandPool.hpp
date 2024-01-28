#pragma once
#include "Base/Device.hpp"

class CommandPool
{
public:
	VkCommandPool vkCommandPool;
	CommandPool(const std::shared_ptr<Device>& device);
	~CommandPool();
};
