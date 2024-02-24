#pragma once
#include "Swapchain.hpp"


enum RenderPassType
{
	GAME_RENDER_PASS = 0,
	EDITOR_RENDER_PASS = 1
};

class RenderPass
{
private:
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	std::shared_ptr<Device> m_device;
	std::shared_ptr<Swapchain> m_swapchain;
	bool m_cleaned = false;

public:
	VkRenderPass vkRenderPass;
	RenderPass(const std::shared_ptr<Device>& device, const std::shared_ptr<Swapchain>& swapchain, RenderPassType renderPassType);
	~RenderPass();
	void CreateEditorRenderPass();
	void CreateGameRenderPass();
	VkRenderPass CreateCubeMapRenderPass();
	VkFormat FindDepthFormat();
	void Cleanup();
};
