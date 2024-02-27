#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
VkFormat FindSupportedFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
