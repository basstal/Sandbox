#include "Subpass.hpp"

#include "Rendering/Base/Device.hpp"
#include "Rendering/Base/Properties.hpp"

static std::map<std::string, std::shared_ptr<VkAttachmentDescription>> attachmentMap;

Subpass::Subpass(const std::shared_ptr<Device>& device, bool inIsMSAASample)
{
	m_device = device;
	isMSAASample = inIsMSAASample;
	hasDepthAttachment = false;
}

Subpass::~Subpass()
{
}

void Subpass::BeginSubpassAttachments()
{
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
			if (isMSAASample && m_referenceIsMSAASamples[index])
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
	vkSubpassDesccription.pResolveAttachments = nullptr;
	if (isMSAASample)
	{
		if (cache.colorAttachmentResolverReferences.size() != cache.colorAttachmentReferences.size())
		{
			throw std::runtime_error("Color attachment and color attachment resolver size mismatch");
		}
		vkSubpassDesccription.pResolveAttachments = cache.colorAttachmentResolverReferences.data();
	}

	subpassDescriptions.push_back(vkSubpassDesccription);
}

void Subpass::AddColorAttachment(std::string name, VkFormat format, VkAttachmentLoadOp loadOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	if (!attachmentMap.contains(name))
	{
		std::shared_ptr<VkAttachmentDescription> attachmentDescription = std::make_shared<VkAttachmentDescription>();
		attachmentDescription->format = format;
		attachmentDescription->samples = isMSAASample ? m_device->msaaSamples : VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription->loadOp = loadOp;
		attachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription->initialLayout = initialLayout;
		attachmentDescription->finalLayout = finalLayout;
		attachmentMap[name] = attachmentDescription;
	}
	if (attachmentMap[name]->format != format)
	{
		throw std::runtime_error("Format mismatch");
	}
	auto it = std::find(attachmentDescriptions.cbegin(), attachmentDescriptions.cend(), attachmentMap[name]);
	if (it == attachmentDescriptions.cend())
	{
		m_referenceAttachments.push_back(static_cast<uint32_t>(attachmentDescriptions.size()));
		m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		attachmentDescriptions.push_back(attachmentMap[name]);
		isAttachmentClearDepthStencil.push_back(false);
		m_referenceIsMSAASamples.push_back(false);
	}
	else
	{
		m_referenceAttachments.push_back(static_cast<uint32_t>(std::distance(attachmentDescriptions.cbegin(), it)));
		m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		m_referenceIsMSAASamples.push_back(false);
	}

	if (isMSAASample)
	{
		AddColorAttachmentResolver(name + "_msaa", format);
	}
}

void Subpass::AddColorAttachmentResolver(std::string name, VkFormat format)
{
	if (!attachmentMap.contains(name))
	{
		std::shared_ptr<VkAttachmentDescription> attachmentDescription = std::make_shared<VkAttachmentDescription>();
		attachmentDescription->format = format;
		attachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription->finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentMap[name] = attachmentDescription;
	}
	if (attachmentMap[name]->format != format)
	{
		throw std::runtime_error("Format mismatch");
	}
	auto it = std::find(attachmentDescriptions.cbegin(), attachmentDescriptions.cend(), attachmentMap[name]);
	if (it == attachmentDescriptions.cend())
	{
		m_referenceAttachments.push_back(static_cast<uint32_t>(attachmentDescriptions.size()));
		m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		attachmentDescriptions.push_back(attachmentMap[name]);
		isAttachmentClearDepthStencil.push_back(false);
		m_referenceIsMSAASamples.push_back(true);
	}
	else
	{
		m_referenceAttachments.push_back(static_cast<uint32_t>(std::distance(attachmentDescriptions.cbegin(), it)));
		m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		m_referenceIsMSAASamples.push_back(true);
	}
}


void Subpass::AssignDepthAttachment(std::string name)
{
	if (!attachmentMap.contains(name))
	{
		std::shared_ptr<VkAttachmentDescription> depthAttachment = std::make_shared<VkAttachmentDescription>();
		depthAttachment->format = FindSupportedFormat(m_device->vkPhysicalDevice, {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
		                                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		depthAttachment->samples = isMSAASample ? m_device->msaaSamples : VK_SAMPLE_COUNT_1_BIT;
		depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachmentMap[name] = depthAttachment;
	}
	if (attachmentMap[name]->finalLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		throw std::runtime_error("Format mismatch");
	}
	for (const auto& layout : m_referenceLayouts)
	{
		if (layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			throw std::runtime_error("Depth attachment already assigned");
		}
	}

	auto it = std::find(attachmentDescriptions.cbegin(), attachmentDescriptions.cend(), attachmentMap[name]);
	if (it == attachmentDescriptions.cend())
	{
		m_referenceAttachments.push_back(static_cast<uint32_t>(attachmentDescriptions.size()));
		m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		attachmentDescriptions.push_back(attachmentMap[name]);
		isAttachmentClearDepthStencil.push_back(true);
		m_referenceIsMSAASamples.push_back(false);
	}
	else
	{
		m_referenceAttachments.push_back(static_cast<uint32_t>(std::distance(attachmentDescriptions.cbegin(), it)));
		m_referenceLayouts.push_back(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		m_referenceIsMSAASamples.push_back(false);
	}
}
