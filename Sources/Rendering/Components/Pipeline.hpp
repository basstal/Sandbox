#pragma once

#include <filesystem>

#include "Rendering/Components/DescriptorResource.hpp"
#include "Rendering/Components/RenderPass.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Objects/Shader.hpp"

class Pipeline
{
private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<RenderPass> m_renderPass;
	std::shared_ptr<Shader> m_shader;
	bool m_cleaned = false;
	bool m_fillModeNonSolid = false;

public:
	// std::vector<VkPipeline> vkPipelines;
	VkPipeline nonSolidPipeline;
	VkPipelineLayout vkPipelineLayout;
	VkPipeline GraphicsPipeline();
	std::shared_ptr<DescriptorResource> descriptorResource;
	VkPipeline vkPipeline;
	Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<Shader>& shader, const std::shared_ptr<RenderPass>& renderPass, VkPrimitiveTopology primitiveTopology,
	         VkPolygonMode polygonMode);
	~Pipeline();
	VkPipelineLayout CreatePipelineLayout(const std::shared_ptr<DescriptorResource>& inDescriptorResource, VkDescriptorSetLayout descriptorSetLayout);
	void ApplySettings(std::shared_ptr<Settings> settings);
	void CreatePipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader);
	void CreateFillModeNonSolidPipeline();
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid);
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
	                          const VkPipelineInputAssemblyStateCreateInfo& inputAssembly);
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const
	                          VkPipelineInputAssemblyStateCreateInfo& inputAssembly, const VkPipelineDepthStencilStateCreateInfo& depthStencil, const VkPipelineLayout& inVkPipelineLayout,
	                          const VkRenderPass& inVkRenderPass, bool
	                          useMultiSampling);
	static VkShaderModule CreateShaderModule(const std::shared_ptr<Device>& device, const std::vector<char>& code);
	void Cleanup();
};
