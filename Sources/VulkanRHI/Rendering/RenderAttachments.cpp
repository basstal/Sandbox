#include "pch.hpp"

#include "RenderAttachments.hpp"

#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"

Sandbox::RenderAttachments::RenderAttachments(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D,
                                              std::vector<std::shared_ptr<ImageView>>& inImageViews)
{
    VkExtent3D extent3D = {extent2D.width, extent2D.height, 1};
    if (!inImageViews.empty() && renderPass->attachments.size() != inImageViews.size())
    {
        LOGF("VulkanRHI", "RenderAttachments::RenderAttachments: renderPass->attachments.size() != inImageViews.size()")
    }
    if (inImageViews.empty())
    {
        inImageViews.resize(renderPass->attachments.size());
    }
    uint32_t index = 0;
    // 资源一一对应
    attachments.resize(renderPass->attachments.size());
    for (auto& imageView : inImageViews)
    {
        if (imageView != nullptr)
        {
            attachments[index++] = imageView->vkImageView;
            continue;
        }
        auto& renderPassAttachment = renderPass->attachments[index];
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
            LOGD("VulkanRHI", "index {} colorImage : {}", std::to_string(index), PtrToHexString(colorImage->vkImage))
            auto colorImageView = std::make_shared<ImageView>(colorImage, VK_IMAGE_VIEW_TYPE_2D);
            images.push_back(colorImage);
            imageViews.push_back(colorImageView);
            attachments[index++] = colorImageView->vkImageView;
        }
        else if (renderPassAttachment.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            auto depthImage =
                std::make_shared<Image>(device, extent3D, renderPassAttachment.format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderPassAttachment.samples);
            auto depthImageView = std::make_shared<ImageView>(depthImage, VK_IMAGE_VIEW_TYPE_2D);
            images.push_back(depthImage);
            imageViews.push_back(depthImageView);
            attachments[index++] = depthImageView->vkImageView;
        }
        else
        {
            LOGF("VulkanRHI", "RenderAttachments::RenderAttachments: renderPassAttachment.usage {} not supported", std::to_string(renderPassAttachment.usage))
        }
    }
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
