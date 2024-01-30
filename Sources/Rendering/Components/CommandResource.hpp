#pragma once
#include "Rendering/Base/Device.hpp"

class CommandResource
{
private:
	std::shared_ptr<Device> m_device;
	bool m_cleaned = false;

public:
	VkCommandPool vkCommandPool;
	std::vector<VkCommandBuffer> vkCommandBuffers;
	CommandResource(const std::shared_ptr<Device>& device);
	~CommandResource();
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	void CreateCommandBuffers();
	void Cleanup();
};
