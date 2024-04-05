#include "pch.hpp"

#include "Device.hpp"

#include "FileSystem/Logger.hpp"
#include "Instance.hpp"
#include "Surface.hpp"

bool Sandbox::QueueFamilyIndices::isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }

Sandbox::Device::Device(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Surface>& surface, const std::vector<const char*>& deviceExtensions)
{
    SelectPhysicalDevice(instance->vkInstance, surface->vkSurfaceKhr, deviceExtensions);
    CreateLogicalDevice(surface->vkSurfaceKhr, deviceExtensions);
}

Sandbox::Device::~Device() { Cleanup(); }

void Sandbox::Device::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyDevice(vkDevice, nullptr);
    m_cleaned = true;
}


void Sandbox::Device::SelectPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface, const std::vector<const char*>& deviceExtensions)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        LOGF_OLD("failed to find GPUs with Vulkan support!")
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        int score = RateDeviceSuitability(device, surface, deviceExtensions);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0)
    {
        vkPhysicalDevice = candidates.rbegin()->second;
    }
    else
    {
        Logger::Fatal("failed to find a suitable GPU!");
    }
}

void Sandbox::Device::CreateLogicalDevice(const VkSurfaceKHR& surface, const std::vector<const char*>& deviceExtensions)
{
    queueFamilyIndices                                           = FindQueueFamilies(vkPhysicalDevice, surface);
    auto                                 swapchainSupportDetails = QuerySwapchainSupport(vkPhysicalDevice, surface);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t>                   uniqueQueueFamilies = {*queueFamilyIndices.graphicsFamily, *queueFamilyIndices.presentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;
    deviceFeatures.fillModeNonSolid  = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos       = queueCreateInfos.data();
    createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledLayerCount       = 0;
    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create logical device!");
    }
    vkGetDeviceQueue(vkDevice, *queueFamilyIndices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(vkDevice, *queueFamilyIndices.presentFamily, 0, &presentQueue);
}

int Sandbox::Device::RateDeviceSuitability(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const std::vector<const char*>& deviceExtensions) const
{
    int                        score = 0;
    VkPhysicalDeviceProperties deviceProperties{};
    VkPhysicalDeviceFeatures   deviceFeatures{};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

    QueueFamilyIndices indices                 = FindQueueFamilies(device, surface);
    bool               swapchainAdequate       = false;
    bool               supportDeviceExtensions = IsDeviceExtensionSupport(device, deviceExtensions);
    if (supportDeviceExtensions)
    {
        SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device, surface);
        swapchainAdequate                        = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    }
    // should support polygon mode line
    bool supportFillModeNonSolid = deviceFeatures.fillModeNonSolid && (deviceProperties.limits.lineWidthRange[0] <= 1.0f && deviceProperties.limits.lineWidthRange[1] >= 1.0f);
    // Application can't function without geometry shaders
    bool supportGeometryShader    = deviceFeatures.geometryShader;
    bool supportSamplerAnisotropy = deviceFeatures.samplerAnisotropy;
    if (!supportGeometryShader || !supportFillModeNonSolid || !indices.isComplete() || !supportDeviceExtensions || !swapchainAdequate || !supportSamplerAnisotropy)
    {
        return 0;
    }

    return score;
}


Sandbox::QueueFamilyIndices Sandbox::Device::FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const
{
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamiliesFinder(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamiliesFinder.data());
    // We need to find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT.
    int i = 0;
    for (const auto& queueFamily : queueFamiliesFinder)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamily = i;
        }
        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    if (!indices.isComplete())
    {
        Logger::Fatal("failed to find suitable queue family!");
    }
    return indices;
}


bool Sandbox::Device::IsDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<const char*>& deviceExtensions) const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}


Sandbox::SwapchainSupportDetails Sandbox::Device::QuerySwapchainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    return details;
}

uint32_t Sandbox::Device::FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    Logger::Fatal("failed to find suitable memory type!");
    return -1;
}

uint32_t Sandbox::Device::GetMaxStorageBuffer() const
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);
    return physicalDeviceProperties.limits.maxStorageBufferRange;
}

uint32_t Sandbox::Device::GetMaxUniformBuffer() const
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);
    return physicalDeviceProperties.limits.maxUniformBufferRange;
}

VkSampleCountFlagBits Sandbox::Device::GetMaxUsableSampleCount() const
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);

    const VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
    {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT)
    {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT)
    {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT)
    {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT)
    {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT)
    {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

uint32_t Sandbox::Device::GetMaxPushConstantsSize() const
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);
    return physicalDeviceProperties.limits.maxPushConstantsSize;
}

float Sandbox::Device::GetMaxAnisotropy() const
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
    return properties.limits.maxSamplerAnisotropy;
}

VkDeviceSize Sandbox::Device::GetMinUniformBufferOffsetAlignment(VkDeviceSize& dynamicAlignment)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
    auto minUboAlignment = properties.limits.minUniformBufferOffsetAlignment;
    if (minUboAlignment > 0)
    {
        dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
    }
    return minUboAlignment;
}
