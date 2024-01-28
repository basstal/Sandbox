﻿#pragma once
#include <memory>

#include "RenderPass.hpp"
#include "Base/Device.hpp"

class Framebuffer
{
private:
	std::shared_ptr<Device> m_device;

public:
	VkFramebuffer vkFramebuffer;
	Framebuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<RenderPass>& renderPass, const VkExtent2D& extent, const std::vector<VkImageView>& attachments);
};
