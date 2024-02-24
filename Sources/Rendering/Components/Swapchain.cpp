#include "Swapchain.hpp"

#include <algorithm>
#include <stdexcept>

#include "Rendering/Application.hpp"

Swapchain::Swapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device)
{
	m_device = device;
	CreateSwapchain(surface, device);
}

Swapchain::~Swapchain()
{
	Cleanup();
}

void Swapchain::CreateSwapchain(const std::shared_ptr<Surface>& surface, const std::shared_ptr<Device>& device)
{
	SwapChainSupportDetails swapChainSupport = device->QuerySwapChainSupport();

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface->vkSurface;
	createInfo.minImageCount = imageCount;
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	VkExtent2D extent = ChooseSwapExtent(surface->glfwWindow, swapChainSupport.capabilities);
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	QueueFamilyIndices indices = device->FindQueueFamilies();
	uint32_t queueFamilyIndices[] = {*indices.graphicsFamily, *indices.presentFamily};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(device->vkDevice, &createInfo, nullptr, &vkSwapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(device->vkDevice, vkSwapchain, &imageCount, nullptr);
	vkImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device->vkDevice, vkSwapchain, &imageCount, vkImages.data());
	vkFormat = surfaceFormat.format;
	vkExtent2D = extent;
	CreateImageViews();
	m_cleaned = false;
}

void Swapchain::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	auto vkDevice = m_device->vkDevice;
	if (m_framebufferCreated)
	{
		vkDestroyImageView(vkDevice, m_vkColorImageView, nullptr);
		vkDestroyImage(vkDevice, m_vkColorImage, nullptr);
		vkFreeMemory(vkDevice, m_vkColorImageMemory, nullptr);

		vkDestroyImageView(vkDevice, m_vkDepthImageView, nullptr);
		vkDestroyImage(vkDevice, m_vkDepthImage, nullptr);
		vkFreeMemory(vkDevice, m_vkDepthImageMemory, nullptr);

		for (auto framebuffer : vkFramebuffers)
		{
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}
		m_framebufferCreated = false;
	}
	for (auto imageView : vkImageViews)
	{
		vkDestroyImageView(vkDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);
	m_cleaned = true;
}

void Swapchain::CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass)
{
	m_device->CreateImage(vkExtent2D.width, vkExtent2D.height, 1, m_device->msaaSamples, vkFormat, VK_IMAGE_TILING_OPTIMAL,
	                      VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkColorImage, m_vkColorImageMemory, false);
	m_vkColorImageView = m_device->CreateImageView(m_vkColorImage, vkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	VkFormat depthFormat = renderPass->FindDepthFormat();
	m_device->CreateImage(vkExtent2D.width, vkExtent2D.height, 1, m_device->msaaSamples, depthFormat,
	                      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkDepthImage, m_vkDepthImageMemory, false);
	m_vkDepthImageView = m_device->CreateImageView(m_vkDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	vkFramebuffers.resize(vkImageViews.size());
	for (size_t i = 0; i < vkImageViews.size(); i++)
	{
		const std::vector<VkImageView> attachments = {
			m_vkColorImageView,
			m_vkDepthImageView,
			vkImageViews[i]
		};
		CreateFramebuffer(vkFramebuffers[i], renderPass->vkRenderPass, attachments);
	}
	m_framebufferCreated = true;
}


void Swapchain::CreateFramebuffer(VkFramebuffer& vkFramebuffer, const VkRenderPass& vkRenderPass, const std::vector<VkImageView>& attachments)
{
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = vkRenderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = vkExtent2D.width;
	framebufferInfo.height = vkExtent2D.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(m_device->vkDevice, &framebufferInfo, nullptr, &vkFramebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
}

VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == COLOR_FORMAT && availableFormat.colorSpace == COLOR_SPACE)
		{
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkExtent2D Swapchain::ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};

	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	return actualExtent;
}


VkPresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

void Swapchain::CreateImageViews()
{
	vkImageViews.resize(vkImages.size());
	for (size_t i = 0; i < vkImages.size(); i++)
	{
		vkImageViews[i] = m_device->CreateImageView(vkImages[i], vkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}
