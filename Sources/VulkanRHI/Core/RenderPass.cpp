#include "pch.hpp"

#include "RenderPass.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Instance.hpp"
#include "Misc/TypeCasting.hpp"
#include "VulkanRHI/Common/Checker.hpp"
#include "VulkanRHI/Common/Debug.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Renderer.hpp"


Sandbox::RenderPass::RenderPass(const std::shared_ptr<Device>& device, const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos,
                                const std::vector<SubpassInfo>& inSubpassInfos) :
    RenderPass(device, inAttachments, inLoadStoreInfos, inSubpassInfos, std::vector{RenderPass::CreateDefaultDependency()})
{
}

Sandbox::RenderPass::RenderPass(const std::shared_ptr<Device>& device, const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos,
                                const std::vector<SubpassInfo>& inSubpassInfos, const std::vector<VkSubpassDependency2KHR>& inSubpassDependency)
{
    // assert(inSubpassInfos.size() == 1 && "Only support one subpass now");
    m_device      = device;
    attachments   = inAttachments;
    loadStoreInfo = inLoadStoreInfos;
    subpasses     = inSubpassInfos;
    CreateRenderPass(inSubpassDependency);
}

void Sandbox::RenderPass::AttachmentReferenceMaker(std::vector<VkAttachmentReference2KHR>& result, const std::vector<uint32_t>& attachmentIndices,
                                                   const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions)
{
    for (auto& index : attachmentIndices)
    {
        const Attachment&         attachment      = attachments[index];
        auto                      referenceLayout = attachment.referenceLayout == VK_IMAGE_LAYOUT_UNDEFINED
                                 ? (attachment.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? attachment.finalLayout : attachment.initialLayout)
                                 : attachment.referenceLayout;
        auto                      description     = attachmentDescriptions[index];
        auto                      aspectMask      = IsDepthFormat(description.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        VkAttachmentReference2KHR reference{};
        reference.sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        reference.attachment = index;
        reference.layout     = referenceLayout;
        reference.aspectMask = aspectMask;
        result.emplace_back(reference);
    }
}
void Sandbox::RenderPass::CreateRenderPass(const std::vector<VkSubpassDependency2KHR>& inSubpassDependency)
{
    std::vector<VkAttachmentDescription2KHR>            attachmentDescriptions = GetAttachmentDescriptions(attachments, loadStoreInfo);
    size_t                                              subpassCount           = subpasses.size();
    std::vector<std::vector<VkAttachmentReference2KHR>> inputAttachmentReferences(subpassCount);
    std::vector<std::vector<VkAttachmentReference2KHR>> colorAttachmentReferences(subpassCount);
    std::vector<std::vector<VkAttachmentReference2KHR>> depthStencilAttachmentReferences(subpassCount);
    std::vector<std::vector<VkAttachmentReference2KHR>> resolveAttachmentReferences(subpassCount);
    std::vector<std::vector<VkAttachmentReference2>>    depthStencilResolveAttachments(subpassCount);
    std::vector<VkSubpassDescription2KHR>               subpassDescriptions;

    for (size_t i = 0; i < subpassCount; ++i)
    {
        auto&                    subpass = subpasses[i];
        VkSubpassDescription2KHR vkSubpassDescription{};
        vkSubpassDescription.sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
        vkSubpassDescription.pNext                   = nullptr;
        vkSubpassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        vkSubpassDescription.preserveAttachmentCount = 0;
        vkSubpassDescription.pPreserveAttachments    = nullptr;
        vkSubpassDescription.flags                   = 0;

        AttachmentReferenceMaker(colorAttachmentReferences[i], subpass.colorAttachments, attachmentDescriptions);
        vkSubpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences[i].size());
        vkSubpassDescription.pColorAttachments    = colorAttachmentReferences[i].empty() ? nullptr : colorAttachmentReferences[i].data();

        AttachmentReferenceMaker(resolveAttachmentReferences[i], subpass.resolveAttachments, attachmentDescriptions);
        vkSubpassDescription.pResolveAttachments = resolveAttachmentReferences[i].empty() ? nullptr : resolveAttachmentReferences[i].data();

        AttachmentReferenceMaker(depthStencilAttachmentReferences[i], subpass.depthStencilAttachments, attachmentDescriptions);
        vkSubpassDescription.pDepthStencilAttachment = depthStencilAttachmentReferences[i].empty() ? nullptr : depthStencilAttachmentReferences[i].data();

        AttachmentReferenceMaker(inputAttachmentReferences[i], subpass.inputAttachments, attachmentDescriptions);
        vkSubpassDescription.inputAttachmentCount = ToUInt32(inputAttachmentReferences[i].size());
        vkSubpassDescription.pInputAttachments    = inputAttachmentReferences[i].empty() ? nullptr : inputAttachmentReferences[i].data();

        if (!subpass.depthStencilResolveAttachments.empty())
        {
            VkSubpassDescriptionDepthStencilResolveKHR subpassDescriptionDepthStencilResolve{};
            subpassDescriptionDepthStencilResolve.sType              = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE_KHR;
            subpassDescriptionDepthStencilResolve.depthResolveMode   = m_device->GetDepthResolveMode(VK_RESOLVE_MODE_MIN_BIT);
            subpassDescriptionDepthStencilResolve.stencilResolveMode = VK_RESOLVE_MODE_NONE;
            AttachmentReferenceMaker(depthStencilResolveAttachments[i], subpass.depthStencilResolveAttachments, attachmentDescriptions);
            subpassDescriptionDepthStencilResolve.pDepthStencilResolveAttachment =
                depthStencilResolveAttachments[i].empty() ? nullptr : depthStencilResolveAttachments[i].data();
            vkSubpassDescription.pNext = &subpassDescriptionDepthStencilResolve;
        }

        subpassDescriptions.push_back(vkSubpassDescription);
    }


    VkRenderPassCreateInfo2KHR renderPassCreateInfo = {};
    renderPassCreateInfo.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
    renderPassCreateInfo.attachmentCount            = ToUInt32(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments               = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount               = ToUInt32(subpassDescriptions.size());
    renderPassCreateInfo.pSubpasses                 = subpassDescriptions.data();
    renderPassCreateInfo.dependencyCount            = ToUInt32(inSubpassDependency.size());
    renderPassCreateInfo.pDependencies              = inSubpassDependency.data();

    // 假设你已有有效的 VkDevice 对象名为 device
    PFN_vkCreateRenderPass2KHR pfnVkCreateRenderPass2KHR = reinterpret_cast<PFN_vkCreateRenderPass2KHR>(vkGetDeviceProcAddr(m_device->vkDevice, "vkCreateRenderPass2KHR"));
    ValidateVkResult(pfnVkCreateRenderPass2KHR(m_device->vkDevice, &renderPassCreateInfo, nullptr, &vkRenderPass));
}

VkSubpassDependency2KHR Sandbox::RenderPass::CreateDefaultDependency()
{
    VkSubpassDependency2KHR dependency{};
    dependency.sType         = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    return dependency;
}

std::vector<VkAttachmentDescription2KHR> Sandbox::RenderPass::GetAttachmentDescriptions(const std::vector<Attachment>&    inAttachments,
                                                                                        const std::vector<LoadStoreInfo>& inLoadStoreInfos)
{
    std::vector<VkAttachmentDescription2KHR> result;
    for (size_t i = 0; i < inAttachments.size(); ++i)
    {
        const Attachment&           attachment = inAttachments[i];
        VkAttachmentDescription2KHR attachmentDescription{};
        attachmentDescription.sType         = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        attachmentDescription.format        = attachment.format;
        attachmentDescription.samples       = attachment.samples;
        attachmentDescription.initialLayout = attachment.initialLayout;
        attachmentDescription.finalLayout   = attachment.finalLayout;
        if (i < inLoadStoreInfos.size())
        {
            attachmentDescription.loadOp         = inLoadStoreInfos[i].loadOp;
            attachmentDescription.storeOp        = inLoadStoreInfos[i].storeOp;
            attachmentDescription.stencilLoadOp  = inLoadStoreInfos[i].loadOp;
            attachmentDescription.stencilStoreOp = inLoadStoreInfos[i].storeOp;
        }

        result.push_back(attachmentDescription);
    }
    return result;
}

Sandbox::RenderPass::~RenderPass() { Cleanup(); }

void Sandbox::RenderPass::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyRenderPass(m_device->vkDevice, vkRenderPass, nullptr);
    m_cleaned = true;
}

std::shared_ptr<Sandbox::Device> Sandbox::RenderPass::GetDevice() const { return m_device; }
