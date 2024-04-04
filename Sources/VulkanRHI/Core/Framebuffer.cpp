#include "pch.hpp"

#include "Framebuffer.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "RenderPass.hpp"

Sandbox::Framebuffer::Framebuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, const std::vector<VkImageView>& imageViews,
                                  VkExtent2D extent2D)
{
    m_device = device;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass->vkRenderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
    framebufferInfo.pAttachments    = imageViews.data();
    framebufferInfo.width           = extent2D.width;
    framebufferInfo.height          = extent2D.height;
    framebufferInfo.layers          = 1;

    if (vkCreateFramebuffer(m_device->vkDevice, &framebufferInfo, nullptr, &vkFramebuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create framebuffer!");
    }
}

Sandbox::Framebuffer::~Framebuffer() { Cleanup(); }

void Sandbox::Framebuffer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyFramebuffer(m_device->vkDevice, vkFramebuffer, nullptr);
    m_cleaned = true;
}
