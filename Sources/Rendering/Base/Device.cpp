#include "Device.hpp"

#include <map>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Properties.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Base/Surface.hpp"

std::vector<const char*> Device::fixedDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME};

bool QueueFamilyIndices::isComplete() const
{
    return graphicsFamily.has_value() && presentFamily.has_value();
}

Device::Device(const VkInstance& instance, const std::shared_ptr<Surface>& surface)
{
    m_surface = surface;
    SelectPhysicalDevice(instance);
    CreateDevice();
}

Device::~Device()
{
    Cleanup();
}

void Device::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyDevice(vkDevice, nullptr);
    m_cleaned = true;
}

void Device::SelectPhysicalDevice(const VkInstance& instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        Logger::Fatal("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        int score = RateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0)
    {
        vkPhysicalDevice = candidates.rbegin()->second;
        msaaSamples = GetMaxUsableSampleCount();
    }
    else
    {
        Logger::Fatal("failed to find a suitable GPU!");
    }
}


int Device::RateDeviceSuitability(const VkPhysicalDevice& device) const
{
    int score = 0;
    VkPhysicalDeviceProperties deviceProperties{};
    VkPhysicalDeviceFeatures deviceFeatures{};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

    QueueFamilyIndices indices = FindQueueFamilies(device);
    bool swapchainAdequate = false;
    bool supportDeviceExtensions = IS_DEVICE_EXTENSION_SUPPORT(device, fixedDeviceExtensions);
    if (supportDeviceExtensions)
    {
        SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device);
        swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    }
    // should support polygon mode line
    bool supportFillModeNonSolid = deviceFeatures.fillModeNonSolid && (deviceProperties.limits.lineWidthRange[0] <= 1.0f && deviceProperties.limits.lineWidthRange[1] >= 1.0f);
    // Application can't function without geometry shaders
    bool supportGeometryShader = deviceFeatures.geometryShader;
    bool supportSamplerAnisotropy = deviceFeatures.samplerAnisotropy;
    if (!supportGeometryShader || !supportFillModeNonSolid || !indices.isComplete() || !supportDeviceExtensions || !swapchainAdequate || !supportSamplerAnisotropy)
    {
        return 0;
    }

    return score;
}

QueueFamilyIndices Device::FindQueueFamilies(const VkPhysicalDevice& device) const
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
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface->vkSurface, &presentSupport);
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


SwapchainSupportDetails Device::QuerySwapchainSupport(const VkPhysicalDevice& device) const
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface->vkSurface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface->vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface->vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface->vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface->vkSurface, &presentModeCount, details.presentModes.data());
    }
    return details;
}

VkSampleCountFlagBits Device::GetMaxUsableSampleCount() const
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


void Device::CreateDevice()
{
    queueFamilies = FindQueueFamilies(vkPhysicalDevice);
    auto swapchainSupportDetails = QuerySwapchainSupport(vkPhysicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {*queueFamilies.graphicsFamily, *queueFamilies.presentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(fixedDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = fixedDeviceExtensions.data();
    createInfo.enabledLayerCount = 0;
    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create logical device!");
    }
    vkGetDeviceQueue(vkDevice, *queueFamilies.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(vkDevice, *queueFamilies.presentFamily, 0, &presentQueue);
}
