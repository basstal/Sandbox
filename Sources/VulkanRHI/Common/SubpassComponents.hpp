#pragma once
#include <vector>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    struct Attachment
    {
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    struct LoadStoreInfo
    {
        VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    };

    struct SubpassInfo
    {
        std::vector<uint32_t> outputAttachments{};
        std::vector<uint32_t> colorResolveAttachments{};
        bool disableDepthStencilAttachment = false;
    };
}
