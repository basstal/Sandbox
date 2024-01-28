﻿#include "Framebuffer.hpp"

#include <stdexcept>

#include "RenderPass.hpp"
Framebuffer::Framebuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, const VkExtent2D& extent, const std::vector<VkImageView>& attachments)
{
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass->vkRenderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device->vkDevice, &framebufferInfo, nullptr, &vkFramebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
}
