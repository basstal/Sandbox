#include "Swapchain.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "ImageView.hpp"
#include "Misc/Debug.hpp"
#include "Platform/Window.hpp"
#include "Semaphore.hpp"
#include "Surface.hpp"
#include "pch.hpp"

Sandbox::Swapchain::Swapchain(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface)
{
    m_device = device;
    m_surface = surface;
    Create(device, surface);
}

Sandbox::Swapchain::~Swapchain() { Cleanup(); }

void Sandbox::Swapchain::Create(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface)
{
    auto swapchainSupports = device->QuerySwapchainSupport(device->vkPhysicalDevice, surface->vkSurfaceKhr);
    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupports.formats);
    uint32_t imageCount;
    ParseCapabilities(surface->window, swapchainSupports.capabilities, imageCount, imageExtent);
    const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupports.presentModes);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->vkSurfaceKhr;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = imageExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    const QueueFamilyIndices indices = device->queueFamilyIndices;
    const uint32_t queueFamilyIndices[] = {*indices.graphicsFamily, *indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    createInfo.preTransform = swapchainSupports.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(m_device->vkDevice, &createInfo, nullptr, &vkSwapchainKhr) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create swap chain!");
    }
    imageViews = CreateImageViews(surfaceFormat.format);
    m_cleaned = false;
}

std::vector<std::shared_ptr<Sandbox::ImageView>> Sandbox::Swapchain::CreateImageViews(VkFormat format)
{
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(m_device->vkDevice, vkSwapchainKhr, &imageCount, nullptr);
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device->vkDevice, vkSwapchainKhr, &imageCount, vkImages.data());
    std::vector<std::shared_ptr<ImageView>> resultImageViews(vkImages.size());
    VkImageSubresource subresource{};
    subresource.mipLevel = 1;
    subresource.arrayLayer = 1;
    for (size_t i = 0; i < vkImages.size(); i++)
    {
        // LOGI("{}", PtrToHexString(vkImages[i]))
        resultImageViews[i] = std::make_shared<ImageView>(m_device, vkImages[i], subresource, VK_IMAGE_VIEW_TYPE_2D, format);
    }
    return resultImageViews;
}

VkSurfaceFormatKHR Sandbox::Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

void Sandbox::Swapchain::ParseCapabilities(const std::shared_ptr<Window>& window, const VkSurfaceCapabilitiesKHR& capabilities, uint32_t& imageCount, VkExtent2D& actualExtent)
{
    imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        actualExtent = capabilities.currentExtent;
        return;
    }
    int width, height;
    glfwGetFramebufferSize(window->glfwWindow, &width, &height);

    actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
}

VkPresentModeKHR Sandbox::Swapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

void Sandbox::Swapchain::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    for (auto& imageView : imageViews)
    {
        imageView->Cleanup();
    }
    vkDestroySwapchainKHR(m_device->vkDevice, vkSwapchainKhr, nullptr);
    m_cleaned = true;
}

Sandbox::ESwapchainStatus Sandbox::Swapchain::AcquireNextImageIndex(const std::shared_ptr<Semaphore>& semaphore)
{
    VkResult result = vkAcquireNextImageKHR(m_device->vkDevice, vkSwapchainKhr, UINT64_MAX, semaphore->vkSemaphore, VK_NULL_HANDLE, &acquiredNextImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        Recreate();
        return ESwapchainStatus::Recreate;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        Logger::Fatal("failed to acquire swap chain image!");
        return ESwapchainStatus::Failure;
    }
    return ESwapchainStatus::Continue;
}

void Sandbox::Swapchain::Recreate()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_surface->window->glfwWindow, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_surface->window->glfwWindow, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_device->vkDevice);

    Cleanup();
    onBeforeRecreateSwapchain.Trigger();
    Create(m_device, m_surface);
    onAfterRecreateSwapchain.Trigger();
    Logger::Debug("RecreateSwapchain finished");
}

Sandbox::ESwapchainStatus Sandbox::Swapchain::Preset(const std::vector<std::shared_ptr<Semaphore>>& inPresentWaitSemaphores)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    std::vector<VkSemaphore> presentWaitSemaphores;
    for (auto it = inPresentWaitSemaphores.cbegin(); it != inPresentWaitSemaphores.cend(); ++it)
    {
        presentWaitSemaphores.push_back((*it)->vkSemaphore);
    }
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(presentWaitSemaphores.size());
    presentInfo.pWaitSemaphores = presentWaitSemaphores.data();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapchainKhr;
    presentInfo.pImageIndices = &acquiredNextImageIndex;
    presentInfo.pResults = nullptr; // Optional
    VkResult result = vkQueuePresentKHR(m_device->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_surface->framebufferResized)
    {
        m_surface->framebufferResized = false;
        Recreate();
    }
    else if (result != VK_SUCCESS)
    {
        Logger::Fatal("failed to present swap chain image!");
        return ESwapchainStatus::Failure;
    }
    return ESwapchainStatus::Continue;
}
