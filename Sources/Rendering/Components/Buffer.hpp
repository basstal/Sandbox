#pragma once
#include <inttypes.h>

#include "Base/Device.hpp"

class Buffer
{
public:
	VkBuffer vkBuffer;
	VkDeviceMemory vkDeviceMemory;
	Buffer(std::shared_ptr<Device> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
};
