#pragma once
#include "Rendering/Base/Device.hpp"
#include "Rendering/Objects/RenderTexture.hpp"
#include "Rendering/Objects/UniformBuffers.hpp"

class DescriptorResource
{
private:
	std::shared_ptr<Device> m_device;
	bool m_cleaned = false;

public:
	VkDescriptorSetLayout vkDescriptorSetLayout;
	VkDescriptorPool vkDescriptorPool;
	std::vector<VkDescriptorSet> vkDescriptorSets;
	DescriptorResource(const std::shared_ptr<Device>& device);
	~DescriptorResource();
	void CreateDescriptorPool();
	void CreateDescriptorSets(const std::shared_ptr<UniformBuffers>& uniformBuffers, const std::shared_ptr<RenderTexture>& renderTexture);
	void Cleanup();
};
