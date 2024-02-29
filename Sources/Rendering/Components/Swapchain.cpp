#include "Swapchain.hpp"
#include <algorithm>
#include <array>

#include "Attachment.hpp"
#include "RenderPass.hpp"
#include "Subpass.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Base/Surface.hpp"
#include "Rendering/Buffers/Image.hpp"
#include "Rendering/Objects/Framebuffer.hpp"

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
    auto swapchainSupports = device->QuerySwapchainSupport(device->vkPhysicalDevice);

    uint32_t imageCount = swapchainSupports.capabilities.minImageCount + 1;
    if (swapchainSupports.capabilities.maxImageCount > 0 && imageCount > swapchainSupports.capabilities.maxImageCount)
    {
        imageCount = swapchainSupports.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->vkSurface;
    createInfo.minImageCount = imageCount;
    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupports.formats);
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    const VkExtent2D extent = ChooseSwapExtent(surface->glfwWindow, swapchainSupports.capabilities);
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    const QueueFamilyIndices indices = device->queueFamilies;
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
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    createInfo.preTransform = swapchainSupports.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupports.presentModes);
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(device->vkDevice, &createInfo, nullptr, &vkSwapchain) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create swap chain!");
    }
    vkGetSwapchainImagesKHR(device->vkDevice, vkSwapchain, &imageCount, nullptr);
    swapchainVkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->vkDevice, vkSwapchain, &imageCount, swapchainVkImages.data());
    swapchainVkFormat = surfaceFormat.format;
    swapchainVkExtent2D = extent;
    swapchainVkImageViews.resize(swapchainVkImages.size());
    for (size_t i = 0; i < swapchainVkImages.size(); i++)
    {
        swapchainVkImageViews[i] = Image::CreateImageView(device->vkDevice, swapchainVkImages[i], swapchainVkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, false);
    }
    m_cleaned = false;
}

void Swapchain::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    auto vkDevice = m_device->vkDevice;
    for (auto& framebuffer : framebuffers)
    {
        framebuffer->Cleanup();
    }
    if (m_colorImage != nullptr)
    {
        m_colorImage->Cleanup();
    }
    if (m_depthImage != nullptr)
    {
        m_depthImage->Cleanup();
    }
    for (auto& swapchainImageView : swapchainVkImageViews)
    {
        vkDestroyImageView(vkDevice, swapchainImageView, nullptr);
    }
    vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);
    m_cleaned = true;
}

void Swapchain::CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass)
{
    if (!renderPass->enableDepth || !renderPass->enableMSAA)
    {
        Logger::Fatal("RenderPass must enable depth and msaa");
    }
    m_colorImage = std::make_shared<Image>(m_device, swapchainVkExtent2D.width, swapchainVkExtent2D.height, 1, m_device->msaaSamples, swapchainVkFormat, VK_IMAGE_TILING_OPTIMAL,
                                           VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false,
                                           VK_IMAGE_ASPECT_COLOR_BIT);
    VkFormat depthFormat = VK_FORMAT_MAX_ENUM;
    for (const auto& attachment : renderPass->subpass->attachments)
    {
        if (attachment->usageDescription == DepthStencil)
        {
            depthFormat = attachment->vkAttachmentDescription.format;
            break;
        }
    }
    m_depthImage = std::make_shared<Image>(m_device, swapchainVkExtent2D.width, swapchainVkExtent2D.height, 1, m_device->msaaSamples, depthFormat,
                                           VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false, VK_IMAGE_ASPECT_DEPTH_BIT);
    framebuffers.resize(swapchainVkImages.size());
    const std::array<VkImageView, 1> shouldAttachedDepthImageViews = {
        m_depthImage->vkImageView
    };
    for (size_t i = 0; i < swapchainVkImages.size(); i++)
    {
        std::array<VkImageView, 2> shouldAttachedColorImageViews = {
            m_colorImage->vkImageView,
            swapchainVkImageViews[i],
        };
        std::vector<VkImageView> attachments;
        int colorAttachmentIndex = 0, depthAttachmentIndex = 0;

        for (const auto& attachment : renderPass->subpass->attachments)
        {
            if (attachment->usageDescription == Color)
            {
                attachments.push_back(shouldAttachedColorImageViews[colorAttachmentIndex++]);
            }
            else if (attachment->usageDescription == DepthStencil)
            {
                attachments.push_back(shouldAttachedDepthImageViews[depthAttachmentIndex++]);
            }
        }
        framebuffers[i] = std::make_shared<Framebuffer>(m_device, renderPass->vkRenderPass, attachments, swapchainVkExtent2D);
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
