#include "pch.hpp"

#include "RenderTarget.hpp"

#include "RenderAttachments.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/Framebuffer.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"

Sandbox::RenderTarget::RenderTarget(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, VkExtent2D inExtent2D,
                                    const std::shared_ptr<RenderAttachments>& attachments):
    extent2D(inExtent2D)
{
    framebuffer = std::make_shared<Framebuffer>(device, renderPass, attachments->attachments, extent2D);
}

Sandbox::RenderTarget::~RenderTarget()
{
    Cleanup();
}

void Sandbox::RenderTarget::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    framebuffer->Cleanup();
    m_cleaned = true;
}
