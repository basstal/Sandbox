#pragma once
#include "DescriptorResource.hpp"

/**
 * @brief 描述 attachment 的作用，
 * 用于在创建 framebuffer 时进行 attachment 排序等操作。
 */
enum AttachmentUsageDescription
{
    Color,
    DepthStencil,
};

/**
 * @brief 包含 VkAttachmentDescription 的结构体。
 */
struct Attachment
{
    /**
     * @brief 作用
     */
    AttachmentUsageDescription usageDescription;
    /**
     * @brief VkAttachmentDescription 结构体
     */
    VkAttachmentDescription vkAttachmentDescription = {};
};
