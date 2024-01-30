#pragma once

#include "Rendering/Components/DescriptorResource.hpp"
#include "Rendering/Components/RenderPass.hpp"
#include "Rendering/Base/Device.hpp"

class Pipeline
{
private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<RenderPass> m_renderPass;
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	bool m_cleaned = false;

public:
	std::vector<VkPipeline> vkPipelines;
	VkPipelineLayout vkPipelineLayout;
	VkPipeline GraphicsPipeline();
	Pipeline(const std::shared_ptr<Device>& device, std::shared_ptr<DescriptorResource> descriptorSet, std::shared_ptr<RenderPass> renderPass);
	~Pipeline();
	void CreatePipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader);
	void Cleanup();
};
