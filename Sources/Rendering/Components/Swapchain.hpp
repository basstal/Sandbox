#pragma once
#include "Base/Device.hpp"

class RenderPass;

class Swapchain
{
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void CreateImageViews();
	void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& vkImage,
	                 VkDeviceMemory& m_vkDeviceMemory);
	std::shared_ptr<Device> m_device;
	VkDeviceMemory m_vkColorImageMemory;
	VkDeviceMemory m_vkDepthImageMemory;

public:
	VkSwapchainKHR vkSwapchain;
	std::vector<VkImage> vkImages;
	VkFormat vkFormat;
	VkExtent2D vkExtent2D;
	std::vector<VkImageView> vkImageViews;
	std::vector<VkFramebuffer> vkFramebuffers;

	Swapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device);
	~Swapchain();
	void CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass);
};
