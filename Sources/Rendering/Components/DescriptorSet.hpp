#pragma once
#include "Base/Device.hpp"

class DescriptorSet
{
public:
	VkDescriptorSetLayout vkDescriptorSetLayout;
	DescriptorSet(const std::shared_ptr<Device>& device);
};
