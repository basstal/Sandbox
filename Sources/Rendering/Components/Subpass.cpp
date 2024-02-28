#include "Subpass.hpp"

#include "Attachment.hpp"
#include "Swapchain.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Base/Properties.hpp"

static std::map<std::string, std::shared_ptr<Attachment>> attachmentMap;

Subpass::Subpass(const std::shared_ptr<Device>& device)
{
    m_device = device;
    isMSAASample = false;
    hasDepthAttachment = false;
    m_samples = VK_SAMPLE_COUNT_1_BIT;
}

Subpass::~Subpass()
{
}

void Subpass::BeginSubpassAttachments(VkSampleCountFlagBits samples)
{
    m_samples = samples;
    m_referenceAttachments.clear();
    m_referenceLayouts.clear();
    m_referenceIsMSAASamples.clear();
}

void Subpass::EndSubpassAttachments()
{
    VkSubpassDescription vkSubpassDesccription;
    vkSubpassDesccription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    vkSubpassDesccription.pDepthStencilAttachment = nullptr;
    m_subpassDescriptionCaches.push_back(SubpassDescriptionCache());
    auto& cache = m_subpassDescriptionCaches.back();
    for (uint32_t index = 0; index < m_referenceLayouts.size(); ++index)
    {
        if (m_referenceLayouts[index] == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            VkAttachmentReference colorAttachmentRef;
            colorAttachmentRef.attachment = m_referenceAttachments[index];
            colorAttachmentRef.layout = m_referenceLayouts[index];
            if (m_referenceIsMSAASamples[index])
            {
                cache.colorAttachmentResolverReferences.push_back(colorAttachmentRef);
            }
            else
            {
                cache.colorAttachmentReferences.push_back(colorAttachmentRef);
            }
        }
        if (m_referenceLayouts[index] == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            hasDepthAttachment = true;
            cache.depthAttachmentReference.attachment = m_referenceAttachments[index];
            cache.depthAttachmentReference.layout = m_referenceLayouts[index];
            vkSubpassDesccription.pDepthStencilAttachment = &cache.depthAttachmentReference;
        }
    }
    vkSubpassDesccription.colorAttachmentCount = static_cast<uint32_t>(cache.colorAttachmentReferences.size());
    vkSubpassDesccription.pColorAttachments = cache.colorAttachmentReferences.data();
    vkSubpassDesccription.inputAttachmentCount = 0;
    vkSubpassDesccription.pInputAttachments = nullptr;
    vkSubpassDesccription.preserveAttachmentCount = 0;
    vkSubpassDesccription.pPreserveAttachments = nullptr;
    vkSubpassDesccription.flags = 0;
    if (!cache.colorAttachmentResolverReferences.empty() && cache.colorAttachmentResolverReferences.size() != cache.colorAttachmentReferences.size())
    {
        Logger::Fatal("Color attachment and color attachment resolver size mismatch");
    }
    vkSubpassDesccription.pResolveAttachments = !cache.colorAttachmentResolverReferences.empty() ? cache.colorAttachmentResolverReferences.data() : nullptr;

    subpassDescriptions.push_back(vkSubpassDesccription);
}

void Subpass::AddColorAttachment(std::string name, VkFormat format, VkAttachmentLoadOp loadOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    if (!attachmentMap.contains(name))
    {
        std::shared_ptr<Attachment> attachment = std::make_shared<Attachment>();
        attachment->vkAttachmentDescription.format = format;
        attachment->vkAttachmentDescription.samples = m_samples;
        attachment->vkAttachmentDescription.loadOp = loadOp;
        attachment->vkAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment->vkAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment->vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment->vkAttachmentDescription.initialLayout = initialLayout;
        attachment->vkAttachmentDescription.finalLayout = finalLayout;
        attachment->usageDescription = Color;
        attachmentMap[name] = attachment;
    }
    if (attachmentMap[name]->vkAttachmentDescription.format != format)
    {
        Logger::Fatal("Color format mismatch");
    }
    auto it = std::find(attachments.cbegin(), attachments.cend(), attachmentMap[name]);
    if (it == attachments.cend())
    {
        m_referenceAttachments.push_back(static_cast<uint32_t>(attachments.size()));
        m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        attachments.push_back(attachmentMap[name]);
        // isAttachmentClearDepthStencil.push_back(false);
        m_referenceIsMSAASamples.push_back(false);
    }
    else
    {
        m_referenceAttachments.push_back(static_cast<uint32_t>(std::distance(attachments.cbegin(), it)));
        m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        m_referenceIsMSAASamples.push_back(false);
    }
}

void Subpass::AddColorAttachmentResolver(std::string name)
{
    isMSAASample = true;
    if (!attachmentMap.contains(name))
    {
        std::shared_ptr<Attachment> attachment = std::make_shared<Attachment>();
        attachment->vkAttachmentDescription.format = FIND_SUPPORTED_FORMAT(m_device->vkPhysicalDevice,
                                                                           {Swapchain::COLOR_FORMAT, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM},
                                                                           VK_IMAGE_TILING_OPTIMAL,
                                                                           VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
        attachment->vkAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment->vkAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment->vkAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment->vkAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment->vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment->vkAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment->vkAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment->usageDescription = Color;
        attachmentMap[name] = attachment;
    }
    auto it = std::find(attachments.cbegin(), attachments.cend(), attachmentMap[name]);
    if (it == attachments.cend())
    {
        m_referenceAttachments.push_back(static_cast<uint32_t>(attachments.size()));
        m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        attachments.push_back(attachmentMap[name]);
        // isAttachmentClearDepthStencil.push_back(false);
        m_referenceIsMSAASamples.push_back(true);
    }
    else
    {
        m_referenceAttachments.push_back(static_cast<uint32_t>(std::distance(attachments.cbegin(), it)));
        m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        m_referenceIsMSAASamples.push_back(true);
    }
}


void Subpass::AssignDepthAttachment(std::string name)
{
    if (!attachmentMap.contains(name))
    {
        std::shared_ptr<Attachment> depthAttachment = std::make_shared<Attachment>();
        depthAttachment->vkAttachmentDescription.format = FIND_SUPPORTED_FORMAT(m_device->vkPhysicalDevice,
                                                                                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                                                                VK_IMAGE_TILING_OPTIMAL,
                                                                                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        depthAttachment->vkAttachmentDescription.samples = m_samples;
        depthAttachment->vkAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment->vkAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment->vkAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment->vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment->vkAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment->vkAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment->usageDescription = DepthStencil;
        attachmentMap[name] = depthAttachment;
    }
    if (attachmentMap[name]->vkAttachmentDescription.finalLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        Logger::Fatal("depth finalLayout mismatch");
    }
    for (const auto& layout : m_referenceLayouts)
    {
        if (layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            Logger::Fatal("Depth attachment already assigned");
        }
    }

    auto it = std::find(attachments.cbegin(), attachments.cend(), attachmentMap[name]);
    if (it == attachments.cend())
    {
        m_referenceAttachments.push_back(static_cast<uint32_t>(attachments.size()));
        m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        attachments.push_back(attachmentMap[name]);
        // isAttachmentClearDepthStencil.push_back(true);
        m_referenceIsMSAASamples.push_back(false);
    }
    else
    {
        m_referenceAttachments.push_back(static_cast<uint32_t>(std::distance(attachments.cbegin(), it)));
        m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        m_referenceIsMSAASamples.push_back(false);
    }
}
