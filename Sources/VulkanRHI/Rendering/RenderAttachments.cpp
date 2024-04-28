#include "pch.hpp"

#include "RenderAttachments.hpp"

#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"

Sandbox::RenderAttachments::RenderAttachments(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D,
                                              const std::vector<std::shared_ptr<ImageView>>& inImageViews)
{
    auto attachmentsCount = renderPass->attachments.size();
    if (!inImageViews.empty() && attachmentsCount != inImageViews.size())
    {
        LOGF("VulkanRHI", "RenderAttachments::RenderAttachments: renderPass->attachments.size() != inImageViews.size()")
    }
    // 资源一一对应
    attachments.resize(attachmentsCount);
    for (size_t i = 0; i < attachmentsCount; ++i)
    {
        if (inImageViews.size() > i)
        {
            auto& imageView = inImageViews[i];
            if (imageView != nullptr)
            {
                attachments[i] = imageView->vkImageView;
            }
        }
    }
    ImageCreation(device, renderPass, extent2D);
}


void Sandbox::RenderAttachments::ImageCreation(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D)
{
    VkExtent3D extent3D         = {extent2D.width, extent2D.height, 1};
    auto       attachmentsCount = renderPass->attachments.size();
    // // 资源一一对应
    // attachments.resize(attachmentsCount);
    for (size_t i = 0; i < attachmentsCount; ++i)
    {
        if (attachments[i] != nullptr)
        {
            continue;
        }
        auto& renderPassAttachment = renderPass->attachments[i];
        if (renderPassAttachment.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            if (renderPassAttachment.samples != VK_SAMPLE_COUNT_1_BIT)
            {
                usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
            }
            else
            {
                usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            }
            auto colorImage = std::make_shared<Image>(device, extent3D, VK_FORMAT_R8G8B8A8_UNORM, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderPassAttachment.samples);
            LOGD("VulkanRHI", "index {} colorImage : {}", std::to_string(i), PtrToHexString(colorImage->vkImage))
            auto colorImageView = std::make_shared<ImageView>(colorImage, VK_IMAGE_VIEW_TYPE_2D);
            images.push_back(colorImage);
            imageViews.push_back(colorImageView);
            attachments[i] = colorImageView->vkImageView;
        }
        else if (renderPassAttachment.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            auto depthImage     = std::make_shared<Image>(device, extent3D, renderPassAttachment.format, renderPassAttachment.usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                      renderPassAttachment.samples);
            auto depthImageView = std::make_shared<ImageView>(depthImage, VK_IMAGE_VIEW_TYPE_2D);
            images.push_back(depthImage);
            imageViews.push_back(depthImageView);
            attachments[i] = depthImageView->vkImageView;
        }
        else
        {
            LOGF("VulkanRHI", "RenderAttachments::RenderAttachments: renderPassAttachment.usage {} not supported", std::to_string(renderPassAttachment.usage))
        }
    }
}
Sandbox::RenderAttachments::RenderAttachments(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D)
{
    auto attachmentsCount = renderPass->attachments.size();
    attachments.resize(attachmentsCount);
    ImageCreation(device, renderPass, extent2D);
}

Sandbox::RenderAttachments::~RenderAttachments() { Cleanup(); }

void Sandbox::RenderAttachments::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    for (auto& imageView : imageViews)
    {
        imageView->Cleanup();
    }
    for (auto& image : images)
    {
        image->Cleanup();
    }
    m_cleaned = true;
}
