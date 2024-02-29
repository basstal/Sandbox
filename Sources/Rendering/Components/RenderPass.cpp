#include "RenderPass.hpp"
#include <memory>
#include "Attachment.hpp"
#include "Subpass.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Objects/Framebuffer.hpp"

RenderPass::RenderPass(const std::shared_ptr<Device>& device, const std::shared_ptr<Subpass>& inSubpass)
{
    m_device = device;
    subpass = inSubpass;
    enableDepth = inSubpass->hasDepthAttachment;
    enableMSAA = inSubpass->isMSAASample;
    std::vector<VkAttachmentDescription> attachments;
    for (const auto& attachment : inSubpass->attachments)
    {
        attachments.push_back(attachment->vkAttachmentDescription);
    }

    VkSubpassDependency dependency{};
    if (enableDepth)
    {
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    else
    {
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(inSubpass->subpassDescriptions.size());
    renderPassCreateInfo.pSubpasses = inSubpass->subpassDescriptions.data();
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;
    if (vkCreateRenderPass(m_device->vkDevice, &renderPassCreateInfo, nullptr, &vkRenderPass) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create editor render pass!");
    }
}

RenderPass::~RenderPass()
{
    Cleanup();
}


void RenderPass::Cleanup()
{
    if (!m_cleaned)
    {
        vkDestroyRenderPass(m_device->vkDevice, vkRenderPass, nullptr);
        m_cleaned = true;
    }
}

void RenderPass::BeginRenderPass(VkCommandBuffer vkCommandBuffer, std::shared_ptr<Framebuffer> framebuffer, VkExtent2D vkExtent2D, VkClearColorValue clearColorValue,
                                 VkClearDepthStencilValue clearDepthStencilValue)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkRenderPass;
    renderPassInfo.framebuffer = framebuffer->vkFramebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkExtent2D;
    std::vector<VkClearValue> clearValues(subpass->attachments.size());
    for (size_t i = 0; i < clearValues.size(); ++i)
    {
        if (subpass->attachments[i]->usageDescription == DepthStencil)
        {
            clearValues[i].depthStencil = clearDepthStencilValue;
        }
        else
        {
            clearValues[i].color = clearColorValue;
        }
    }
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
