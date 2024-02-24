#pragma once

#include <filesystem>

#include "Rendering/Components/DescriptorResource.hpp"
#include "Rendering/Components/RenderPass.hpp"
#include "Rendering/Base/Device.hpp"

class Pipeline
{
private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<RenderPass> m_renderPass;
	std::shared_ptr<DescriptorResource> m_descriptorResource;
	bool m_cleaned = false;
	bool m_fillModeNonSolid = false;

public:
	std::vector<VkPipeline> vkPipelines;
	VkPipeline nonSolidPipeline;
	VkPipelineLayout vkPipelineLayout;
	VkPipeline GraphicsPipeline();
	Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<DescriptorResource>& descriptorResource, const std::shared_ptr<RenderPass>& renderPass);
	~Pipeline();
	VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
	void ApplySettings(std::shared_ptr<Settings> settings);
	void CreatePipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader);
	void CreateFillModeNonSolidPipeline();
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid);
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
	                          const VkPipelineInputAssemblyStateCreateInfo& inputAssembly);
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const
	                          VkPipelineInputAssemblyStateCreateInfo& inputAssembly, const VkPipelineDepthStencilStateCreateInfo& depthStencil, const VkPipelineLayout& inVkPipelineLayout, const VkRenderPass& inVkRenderPass, bool
	                          useMultiSampling);
	static VkShaderModule CreateShaderModule(const std::shared_ptr<Device>& device, const std::vector<char>& code);
	void Cleanup();
};
