#pragma once
#include "DescriptorResource.hpp"

enum AttachmentUsageDescription
{
    Color,
    DepthStencil,
};

class Attachment
{
public:
    AttachmentUsageDescription usageDescription;
    VkAttachmentDescription vkAttachmentDescription;
};
