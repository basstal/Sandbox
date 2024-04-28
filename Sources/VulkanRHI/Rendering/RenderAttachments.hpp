#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Sandbox
{
    class ImageView;
    class Image;
    class Device;
    class RenderPass;

    class RenderAttachments
    {
    public:
        RenderAttachments(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D,
                          const std::vector<std::shared_ptr<ImageView>>& inImageViews);
        RenderAttachments(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D);

        ~RenderAttachments();

        void ImageCreation(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D);

        void Cleanup();

        std::vector<std::shared_ptr<Image>>     images;
        std::vector<std::shared_ptr<ImageView>> imageViews;

        std::vector<VkImageView> attachments;

    private:
        bool m_cleaned = false;
    };
}  // namespace Sandbox
