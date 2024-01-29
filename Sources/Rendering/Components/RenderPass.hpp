#pragma once
#include "Swapchain.hpp"

class RenderPass
{
private:
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	std::shared_ptr<Device> m_device;
	bool m_cleaned = false;

public:
	VkRenderPass vkRenderPass;
	RenderPass(const std::shared_ptr<Device>& device, const std::shared_ptr<Swapchain>& swapchain);
	~RenderPass();
	VkFormat FindDepthFormat();
	void Cleanup();
};
