#pragma once
#include <inttypes.h>

#include "Rendering/Base/Device.hpp"

class Buffer
{
private:
	bool m_cleaned = false;
	std::shared_ptr<Device> m_device;
public:
	VkBuffer vkBuffer;
	VkDeviceMemory vkDeviceMemory;
	Buffer(std::shared_ptr<Device> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~Buffer();
	void Cleanup();
};
