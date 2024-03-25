#include "pch.hpp"

#include "RenderAttachments.hpp"

#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"

Sandbox::RenderAttachments::RenderAttachments(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D,
                                              const std::shared_ptr<ImageView>& inResolveImageView)
{
    VkExtent3D extent3D = {extent2D.width, extent2D.height, 1};
    if (renderPass->attachments.size() > 1) // 如果只有一个 imageView，则不创建额外的 color 和 depth attachment 对象
    {
        colorImage = std::make_shared<Image>(device, extent3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderPass->attachments[0].samples);
        colorImageView = std::make_shared<ImageView>(colorImage, VK_IMAGE_VIEW_TYPE_2D);
        attachments.push_back(colorImageView->vkImageView);
        auto enableDepth = !renderPass->subpasses[0].disableDepthStencilAttachment;
        if (enableDepth)
        {
            VkFormat depthFormat = VK_FORMAT_UNDEFINED;
            VkSampleCountFlagBits depthSamples = VK_SAMPLE_COUNT_1_BIT;
            for (const Attachment& attachment : renderPass->attachments)
            {
                if (attachment.usage == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                {
                    depthFormat = attachment.format;
                    depthSamples = attachment.samples;
                    break;
                }
            }
            depthImage = std::make_shared<Image>(device, extent3D, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                 depthSamples);
            depthImageView = std::make_shared<ImageView>(depthImage, VK_IMAGE_VIEW_TYPE_2D);
            attachments.push_back(depthImageView->vkImageView);
        }
    }
    if (inResolveImageView == nullptr)
    {
        resolveImage = std::make_shared<Image>(device, extent3D, VK_FORMAT_R8G8B8A8_UNORM,
                                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);
        resolveImageView = std::make_shared<ImageView>(resolveImage, VK_IMAGE_VIEW_TYPE_2D);
        attachments.push_back(resolveImageView->vkImageView);
    }
    else
    {
        attachments.push_back(inResolveImageView->vkImageView);
    }
}

Sandbox::RenderAttachments::~RenderAttachments()
{
    Cleanup();
}

void Sandbox::RenderAttachments::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    resolveImageView != nullptr ? resolveImageView->Cleanup() : void();
    resolveImage != nullptr ? resolveImage->Cleanup() : void();
    depthImageView != nullptr ? depthImageView->Cleanup() : void();
    depthImage != nullptr ? depthImage->Cleanup() : void();
    colorImageView != nullptr ? colorImageView->Cleanup() : void();
    colorImage != nullptr ? colorImage->Cleanup() : void();
    m_cleaned = true;
}
