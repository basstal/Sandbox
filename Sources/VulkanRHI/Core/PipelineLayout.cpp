#include "pch.hpp"

#include "PipelineLayout.hpp"

#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "ShaderModule.hpp"

Sandbox::PipelineLayout::PipelineLayout(const std::shared_ptr<Device>& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules)
{
    m_device = device;

    descriptorSetLayout = std::make_shared<DescriptorSetLayout>(device, shaderModules);

    for (const std::shared_ptr<ShaderModule>& shaderModule : shaderModules)
    {
        shaderModule->ReflectPushConstantRanges(pushConstantRanges);
    }
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 1;
    pipelineLayoutInfo.pSetLayouts            = &descriptorSetLayout->vkDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutInfo.pPushConstantRanges    = pushConstantRanges.data();

    if (vkCreatePipelineLayout(m_device->vkDevice, &pipelineLayoutInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create pipeline layout!");
    }
}

Sandbox::PipelineLayout::~PipelineLayout() { Cleanup(); }

void Sandbox::PipelineLayout::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    descriptorSetLayout->Cleanup();
    vkDestroyPipelineLayout(m_device->vkDevice, vkPipelineLayout, nullptr);
    m_cleaned = true;
}
