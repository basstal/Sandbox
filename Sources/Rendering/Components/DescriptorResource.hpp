#pragma once
#include "Rendering/Material.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Objects/RenderTexture.hpp"
#include "Rendering/Buffers/UniformBuffers.hpp"

struct Uniform;

class DescriptorResource
{
private:
	std::shared_ptr<Device> m_device;
	bool m_cleaned = false;
	static const uint32_t DESCRIPTOR_POOL_SIZE = 2048;

public:
	static VkDescriptorPool vkDescriptorPool;
	VkDescriptorSetLayout vkDescriptorSetLayout;
	std::vector<VkDescriptorSet> vkDescriptorSets;
	std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
	std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescriptions;
	std::vector<VkPushConstantRange> vkPushConstantRanges;
	std::map<std::string, int32_t> nameToBinding;
	VkVertexInputBindingDescription vkVertexInputBindingDescription;
	DescriptorResource(const std::shared_ptr<Device>& device);
	~DescriptorResource();
	VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	static void CreateDescriptorPool(const std::shared_ptr<Device>& device);
	void CreateDescriptorSets();
	void Cleanup();
};
