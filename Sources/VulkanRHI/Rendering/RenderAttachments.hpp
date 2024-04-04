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
                          const std::shared_ptr<ImageView>& inResolveImageView);

        ~RenderAttachments();

        void Cleanup();

        std::shared_ptr<Image>     colorImage;
        std::shared_ptr<ImageView> colorImageView;
        std::shared_ptr<Image>     depthImage;
        std::shared_ptr<ImageView> depthImageView;
        std::shared_ptr<Image>     resolveImage;
        std::shared_ptr<ImageView> resolveImageView;

        std::vector<VkImageView> attachments;

    private:
        bool m_cleaned = false;
    };
}  // namespace Sandbox
