#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Rendering/Base/Device.hpp"


class Subpass
{
private:
	struct SubpassDescriptionCache
	{
		std::vector<VkAttachmentReference> colorAttachmentReferences;
		std::vector<VkAttachmentReference> colorAttachmentResolverReferences;
		VkAttachmentReference depthAttachmentReference;
	};

	std::shared_ptr<Device> m_device;
	std::vector<uint32_t> m_referenceAttachments;
	std::vector<VkImageLayout> m_referenceLayouts;
	std::vector<bool> m_referenceIsMSAASamples;
	std::vector<SubpassDescriptionCache> m_subpassDescriptionCaches;

public:
	bool isMSAASample;
	bool hasDepthAttachment;
	std::vector<bool> isAttachmentClearDepthStencil;
	std::vector<std::shared_ptr<VkAttachmentDescription>> attachmentDescriptions;
	std::vector<VkSubpassDescription> subpassDescriptions;
	Subpass(const std::shared_ptr<Device>& device, bool isMSAASample);
	~Subpass();
	void BeginSubpassAttachments();
	void EndSubpassAttachments();
	void AddColorAttachment(std::string name, VkFormat format, VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	                        VkImageLayout finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	void AddColorAttachmentResolver(std::string name, VkFormat format);
	void AssignDepthAttachment(std::string name);
};
