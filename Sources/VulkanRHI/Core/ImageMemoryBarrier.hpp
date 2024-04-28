#pragma once

namespace Sandbox
{
    class ImageMemoryBarrier
    {
    public:
        VkPipelineStageFlags srcStageMask{VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};

        VkPipelineStageFlags dstStageMask{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

        VkAccessFlags srcAccessMask{0};

        VkAccessFlags dstAccessMask{0};

        VkImageLayout oldLayout{VK_IMAGE_LAYOUT_UNDEFINED};

        VkImageLayout newLayout{VK_IMAGE_LAYOUT_UNDEFINED};

        uint32_t oldQueueFamily{VK_QUEUE_FAMILY_IGNORED};

        uint32_t newQueueFamily{VK_QUEUE_FAMILY_IGNORED};
    };
}  // namespace Sandbox
