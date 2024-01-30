#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Rendering/Base/Device.hpp"

class SyncObjects
{
private:
	bool m_cleanup = false;
	std::shared_ptr<Device> m_device;

public:
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	SyncObjects(const std::shared_ptr<Device>& device);
	~SyncObjects();
	void Cleanup();
};
