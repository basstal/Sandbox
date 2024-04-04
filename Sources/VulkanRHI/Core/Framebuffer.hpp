#pragma once
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;
    class RenderPass;

    class Framebuffer
    {
    public:
        Framebuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent2D);

        ~Framebuffer();

        void Cleanup();

        VkFramebuffer vkFramebuffer;

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
