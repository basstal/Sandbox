#pragma once
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class RenderAttachments;
    class Device;
    class RenderPass;
    class Framebuffer;
    class Image;
    class ImageView;

    class RenderTarget
    {
    public:
        RenderTarget(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D extent2D,
                     const std::shared_ptr<RenderAttachments>& attachments);

        ~RenderTarget();

        void Cleanup();

        std::shared_ptr<Framebuffer> framebuffer;

        VkExtent2D extent2D;

    private:
        bool m_cleaned = false;
    };
}  // namespace Sandbox
