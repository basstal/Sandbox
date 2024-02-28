#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

/**
 * \brief 查找物理设备支持的内存类型
 * \param physicalDevice 物理设备
 * \param typeFilter 过滤类型
 * \param properties 属性
 * \return 内存类型信息
 */
uint32_t FIND_MEMORY_TYPE(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

/**
 * \brief 查找物理设备支持的格式
 * \param vkPhysicalDevice 物理设备
 * \param candidates 格式列表
 * \param tiling 图像平铺
 * \param features 特性
 * \return 支持的格式
 */
VkFormat FIND_SUPPORTED_FORMAT(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

/**
 * \brief 检查物理设备是否支持扩展列表中的所有扩展
 * \param device 物理设备
 * \param deviceExtensions 扩展列表
 * \return 是否支持
 */
bool IS_DEVICE_EXTENSION_SUPPORT(const VkPhysicalDevice& device, const std::vector<const char*>& deviceExtensions);
