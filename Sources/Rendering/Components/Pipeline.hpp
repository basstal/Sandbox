#pragma once

#include "DescriptorSet.hpp"
#include "RenderPass.hpp"
#include "Base/Device.hpp"

class Pipeline
{
private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<RenderPass> m_renderPass;
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

public:
	std::vector<VkPipeline> vkPipelines;
	VkPipelineLayout vkPipelineLayout;
	Pipeline(const std::shared_ptr<Device>& device, std::shared_ptr<DescriptorSet> descriptorSet, std::shared_ptr<RenderPass> renderPass);
	~Pipeline();
	void CreatePipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader);
};
