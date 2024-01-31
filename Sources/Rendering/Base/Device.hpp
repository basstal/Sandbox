#pragma once
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Surface.hpp"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete();
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Device
{
private:
	std::shared_ptr<Surface> m_surface;
	int RateDeviceSuitability(const VkPhysicalDevice& device);
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
	bool IsDeviceExtensionSupport(const VkPhysicalDevice& device);
	SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device);
	VkSampleCountFlagBits GetMaxUsableSampleCount();
	void CreatePhysicalDevice(const VkInstance& instance);
	void CreateDevice();
	bool m_cleaned = false;

public:
	static const size_t MAX_FRAMES_IN_FLIGHT = 2;
	VkPhysicalDevice vkPhysicalDevice;
	VkDevice vkDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSampleCountFlagBits msaaSamples;
	QueueFamilyIndices queueFamilies;
	Device(const VkInstance& instance, const std::shared_ptr<Surface>& surface);
	~Device();
	const std::vector<const char*> GetDeviceExtensions() const;
	QueueFamilyIndices FindQueueFamilies();
	SwapChainSupportDetails QuerySwapChainSupport();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void Cleanup();
};
