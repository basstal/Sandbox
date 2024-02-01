#pragma once
#include "Rendering/Base/Device.hpp"

class RenderPass;

class Swapchain
{
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void CreateImageViews();
	std::shared_ptr<Device> m_device;
	VkImage m_vkColorImage;
	VkImageView m_vkColorImageView;
	VkDeviceMemory m_vkColorImageMemory;
	VkImage m_vkDepthImage;
	VkImageView m_vkDepthImageView;
	VkDeviceMemory m_vkDepthImageMemory;

	bool m_cleaned = false;
	bool m_framebufferCreated = false;

public:
	static const VkFormat COLOR_FORMAT = VK_FORMAT_B8G8R8A8_UNORM;
	static const VkColorSpaceKHR COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkSwapchainKHR vkSwapchain;
	VkFormat vkFormat;
	VkExtent2D vkExtent2D;
	std::vector<VkImage> vkImages;
	std::vector<VkImageView> vkImageViews;
	std::vector<VkFramebuffer> vkFramebuffers;

	Swapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device);
	~Swapchain();
	void Cleanup();
	void CreateSwapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device);
	void CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass);
	void CreateFramebuffer(VkFramebuffer& vkFramebuffer, const VkRenderPass& vkRenderPass, const std::vector<VkImageView>& attachments);
};
