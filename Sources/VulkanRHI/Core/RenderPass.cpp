#include "pch.hpp"

#include "RenderPass.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"

bool IsDepthFormat(VkFormat format)
{
    return format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT // only depth
        || format == VK_FORMAT_D16_UNORM_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT; // depth and stencil
}

Sandbox::RenderPass::RenderPass(const std::shared_ptr<Device>& device, const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos,
                                const std::vector<SubpassInfo>& inSubpassInfos):
    RenderPass(device, inAttachments, inLoadStoreInfos, inSubpassInfos, CreateDefaultDependency())
{
}

Sandbox::RenderPass::RenderPass(const std::shared_ptr<Device>& device, const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos,
                                const std::vector<SubpassInfo>& inSubpassInfos, VkSubpassDependency inSubpassDependency)
{
    assert(inSubpassInfos.size() == 1 && "Only support one subpass now");
    m_device = device;
    attachments = inAttachments;
    loadStoreInfo = inLoadStoreInfos;
    subpasses = inSubpassInfos;
    std::vector<VkAttachmentDescription> attachmentDescriptions = GetAttachmentDescriptions(inAttachments, inLoadStoreInfos);
    std::vector<std::vector<VkAttachmentReference>> colorAttachmentReferences;
    std::vector<std::vector<VkAttachmentReference>> depthStencilAttachmentReferences;
    std::vector<std::vector<VkAttachmentReference>> colorResolveAttachmentReferences;
    size_t subpassCount = inSubpassInfos.size();
    colorAttachmentReferences.resize(subpassCount);
    depthStencilAttachmentReferences.resize(subpassCount);
    colorResolveAttachmentReferences.resize(subpassCount);
    for (size_t i = 0; i < subpassCount; ++i)
    {
        auto& subpass = inSubpassInfos[i];
        for (auto outputAttachmentIndex : subpass.outputAttachments)
        {
            const Attachment& attachment = inAttachments[outputAttachmentIndex];
            auto initialLayout = attachment.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : attachment.initialLayout;
            auto& description = attachmentDescriptions[outputAttachmentIndex];

            if (!IsDepthFormat(description.format))
            {
                colorAttachmentReferences[i].push_back(VkAttachmentReference{outputAttachmentIndex, initialLayout});
            }
        }

        for (auto colorResolveAttachmentIndex : subpass.colorResolveAttachments)
        {
            const Attachment& attachment = inAttachments[colorResolveAttachmentIndex];
            auto initialLayout = attachment.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : attachment.initialLayout;
            colorResolveAttachmentReferences[i].push_back(VkAttachmentReference{colorResolveAttachmentIndex, initialLayout});
        }

        if (!subpass.disableDepthStencilAttachment)
        {
            auto it = std::find_if(inAttachments.begin(), inAttachments.end(), [this](const Attachment& attachment) { return IsDepthFormat(attachment.format); });
            if (it != inAttachments.end())
            {
                auto depthStencilIndex = static_cast<uint32_t>(std::distance(inAttachments.begin(), it));
                auto initialLayout = it->initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : it->initialLayout;
                depthStencilAttachmentReferences[i].push_back(VkAttachmentReference{depthStencilIndex, initialLayout});
            }
        }
    }

    std::vector<VkSubpassDescription> subpassDescriptions;
    for (size_t i = 0; i < subpassCount; ++i)
    {
        VkSubpassDescription vkSubpassDescription;
        vkSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        vkSubpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences[i].size());
        vkSubpassDescription.pColorAttachments = colorAttachmentReferences[i].empty() ? nullptr : colorAttachmentReferences[i].data();
        vkSubpassDescription.inputAttachmentCount = 0;
        vkSubpassDescription.pInputAttachments = nullptr;
        vkSubpassDescription.preserveAttachmentCount = 0;
        vkSubpassDescription.pPreserveAttachments = nullptr;
        vkSubpassDescription.pDepthStencilAttachment = depthStencilAttachmentReferences[i].empty() ? nullptr : depthStencilAttachmentReferences[i].data();
        vkSubpassDescription.pResolveAttachments = colorResolveAttachmentReferences[i].empty() ? nullptr : colorResolveAttachmentReferences[i].data();
        vkSubpassDescription.flags = 0;
        subpassDescriptions.push_back(vkSubpassDescription);
    }

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &inSubpassDependency;

    if (vkCreateRenderPass(m_device->vkDevice, &renderPassCreateInfo, nullptr, &vkRenderPass) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create editor render pass!");
    }
}

VkSubpassDependency Sandbox::RenderPass::CreateDefaultDependency()
{
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    return dependency;
}

std::vector<VkAttachmentDescription> Sandbox::RenderPass::GetAttachmentDescriptions(const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos)
{
    std::vector<VkAttachmentDescription> result;
    for (size_t i = 0; i < inAttachments.size(); ++i)
    {
        const Attachment& attachment = inAttachments[i];
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = attachment.format;
        attachmentDescription.samples = attachment.samples;
        attachmentDescription.initialLayout = attachment.initialLayout;
        attachmentDescription.finalLayout = IsDepthFormat(attachmentDescription.format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : attachment.finalLayout;
        if (i < inLoadStoreInfos.size())
        {
            attachmentDescription.loadOp = inLoadStoreInfos[i].loadOp;
            attachmentDescription.storeOp = inLoadStoreInfos[i].storeOp;
            // TODO: enable stencil load store
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // inLoadStoreInfos[i].loadOp;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // inLoadStoreInfos[i].storeOp;
        }

        result.push_back(attachmentDescription);
    }
    return result;
}

Sandbox::RenderPass::~RenderPass()
{
    Cleanup();
}

void Sandbox::RenderPass::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyRenderPass(m_device->vkDevice, vkRenderPass, nullptr);
    m_cleaned = true;
}
