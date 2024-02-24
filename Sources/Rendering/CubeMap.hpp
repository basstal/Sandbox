#pragma once
#include <memory>

#include "Base/Device.hpp"
#include "Components/Buffer.hpp"
#include "Objects/RenderTexture.hpp"

class Pipeline;
class RenderPass;
class DescriptorResource;
class UniformBuffers;

class CubeMap
{
private:
	std::shared_ptr<Device> m_device;
	bool m_cleaned = false;
	VkVertexInputBindingDescription GetBindingDescription();
	std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
	std::array<VkDeviceMemory, 6> vkDeviceMemoriesCubeMap;

	std::array<VkImageView, 6> vkImageViewCubeMap;
	std::array<VkImage, 6> vkImageCubeMap;

public:
	std::shared_ptr<RenderTexture> renderTexture;
	std::shared_ptr<RenderTexture> irradianceMap;
	std::shared_ptr<Buffer> vertexBuffer;
	VkPipeline vkPipeline;
	VkPipelineLayout vkPipelineLayout;
	VkRenderPass vkRenderPass;
	uint32_t size;
	std::array<VkFramebuffer, 6> vkFramebuffers;
	VkDescriptorSetLayout vkDescriptorSetLayout;
	VkDescriptorSet vkDescriptorSet;
	CubeMap(const std::shared_ptr<Device>& device, const std::shared_ptr<Image>& image, const std::shared_ptr<CommandResource>& commandResource, uint32_t inSize,
	        const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<DescriptorResource>& descriptorResource);
	~CubeMap();
	void Cleanup();
	void CreateDescriptorSet(const std::shared_ptr<UniformBuffers>& uniformBuffers, const std::shared_ptr<DescriptorResource>& descriptorResource);
	void RenderCube(const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<UniformBuffers>& uniformBuffers, const std::shared_ptr<DescriptorResource>& descriptorResource);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandResource>& commandResource);
};
