#pragma once

#include "Rendering/Components/DescriptorResource.hpp"
#include "Rendering/Components/RenderPass.hpp"
#include "Rendering/Base/Device.hpp"

class Pipeline
{
private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<RenderPass> m_renderPass;
	std::shared_ptr<DescriptorResource> m_descriptorResource;
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	bool m_cleaned = false;
	bool m_fillModeNonSolid = false;

public:
	std::vector<VkPipeline> vkPipelines;
	VkPipeline nonSolidPipeline;
	VkPipelineLayout vkPipelineLayout;
	VkPipeline GraphicsPipeline();
	Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<DescriptorResource>& descriptorResource, const std::shared_ptr<RenderPass>& renderPass);
	~Pipeline();
	void CreatePipelineLayout();
	void ApplySettings(std::shared_ptr<Settings> settings);
	void CreatePipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader);
	void CreateFillModeNonSolidPipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader);
	VkPipeline CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid);
	void Cleanup();
};
