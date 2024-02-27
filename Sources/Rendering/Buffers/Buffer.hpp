#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

class Device;

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
