#include "RenderPass.hpp"

#include <memory>
#include <stdexcept>

#include "Attachment.hpp"
#include "Subpass.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Base/Properties.hpp"

RenderPass::RenderPass(const std::shared_ptr<Device>& device, const std::shared_ptr<Subpass>& inSubpass)
{
    m_device = device;
    subpass = inSubpass;
    enableDepth = inSubpass->hasDepthAttachment;
    enableMSAA = inSubpass->isMSAASample;
    for (const auto& attachment : inSubpass->attachments)
    {
        // isAttachmentClearDepthStencil = isAttachmentClearDepthStencil || attachment->usageDescription == DepthStencil;
        attachments.push_back(attachment->vkAttachmentDescription);
    }
    // isAttachmentClearDepthStencil = inSubpass->isAttachmentClearDepthStencil;

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

void RenderPass::CreateEditorRenderPass()
{
    // VkAttachmentDescription attachment = {};
    // attachment.format = m_swapchain->swapchainVkFormat;
    // attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    // VkAttachmentReference colorAttachment;
    // colorAttachment.attachment = 0;
    // colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // VkSubpassDescription subpass = {};
    // subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // subpass.colorAttachmentCount = 1;
    // subpass.pColorAttachments = &colorAttachment;
    // VkSubpassDependency dependency = {};
    // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependency.dstSubpass = 0;
    // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // dependency.srcAccessMask = 0;
    // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // VkRenderPassCreateInfo info = {};
    // info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // info.attachmentCount = 1;
    // info.pAttachments = &attachment;
    // info.subpassCount = 1;
    // info.pSubpasses = &subpass;
    // info.dependencyCount = 1;
    // info.pDependencies = &dependency;
    // if (vkCreateRenderPass(m_device->vkDevice, &info, nullptr, &vkRenderPass) != VK_SUCCESS)
    // {
    //     Logger::Fatal("failed to create editor render pass!");
    // }
}

VkRenderPass RenderPass::CreateCubeMapRenderPass()
{
    // VkAttachmentDescription colorAttachment = {};
    // colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT; // 例如，对于高动态范围立方图
    // colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // 不使用多重采样
    // colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // 开始渲染时清除附件
    // colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // 渲染结束时保存附件数据
    // colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 模板缓冲区不重要
    // colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 模板缓冲区不重要
    // colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // 开始时不关心图像的布局
    // colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 渲染结束时将图像转换为颜色附件布局
    //
    // VkAttachmentReference colorAttachmentRef;
    // colorAttachmentRef.attachment = 0; // 附件描述符数组中的索引
    // colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 在子通道期间使用的布局
    //
    // VkSubpassDescription subpass = {};
    // subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // subpass.colorAttachmentCount = 1;
    // subpass.pColorAttachments = &colorAttachmentRef;
    //
    // VkRenderPassCreateInfo renderPassInfo = {};
    // renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // renderPassInfo.attachmentCount = 1;
    // renderPassInfo.pAttachments = &colorAttachment;
    // renderPassInfo.subpassCount = 1;
    // renderPassInfo.pSubpasses = &subpass;
    //
    // VkSubpassDependency dependency{};
    // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependency.dstSubpass = 0;
    // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    // dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    //
    // renderPassInfo.dependencyCount = 1;
    // renderPassInfo.pDependencies = &dependency;
    // VkRenderPass renderPass;
    // if (vkCreateRenderPass(m_device->vkDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    // {
    //     Logger::Fatal("failed to create render pass!");
    // }
    // return renderPass;
    return nullptr;
}

void RenderPass::CreateGameRenderPass()
{
    // VkAttachmentDescription colorAttachment{};
    // colorAttachment.format = m_swapchain->swapchainVkFormat;
    // colorAttachment.samples = m_device->msaaSamples;
    // colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //
    // colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentDescription depthAttachment{};
    // depthAttachment.format = FindDepthFormat();
    // depthAttachment.samples = m_device->msaaSamples;
    // depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentDescription colorAttachmentResolve{};
    // colorAttachmentResolve.format = m_swapchain->swapchainVkFormat;
    // colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    // colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentReference colorAttachmentResolveRef{};
    // colorAttachmentResolveRef.attachment = 2;
    // colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentReference depthAttachmentRef{};
    // depthAttachmentRef.attachment = 1;
    // depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentReference colorAttachmentRef{};
    // colorAttachmentRef.attachment = 0;
    // colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //
    // VkSubpassDescription subpass{};
    // subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // subpass.colorAttachmentCount = 1;
    // subpass.pColorAttachments = &colorAttachmentRef;
    // subpass.pDepthStencilAttachment = &depthAttachmentRef;
    // subpass.pResolveAttachments = &colorAttachmentResolveRef;
    //
    // std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
    // VkRenderPassCreateInfo renderPassInfo{};
    // renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    // renderPassInfo.pAttachments = attachments.data();
    // renderPassInfo.subpassCount = 1;
    // renderPassInfo.pSubpasses = &subpass;
    //
    // VkSubpassDependency dependency{};
    // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependency.dstSubpass = 0;
    // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    // dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    //
    // renderPassInfo.dependencyCount = 1;
    // renderPassInfo.pDependencies = &dependency;
    // if (vkCreateRenderPass(m_device->vkDevice, &renderPassInfo, nullptr, &vkRenderPass) != VK_SUCCESS)
    // {
    // 	Logger::Fatal("failed to create game render pass!");
    // }
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
    std::vector<VkClearValue> clearValues(attachments.size());
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
