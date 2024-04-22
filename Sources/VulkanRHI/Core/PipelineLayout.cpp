#include "pch.hpp"

#include "PipelineLayout.hpp"

#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "ShaderModule.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"

Sandbox::PipelineLayout::PipelineLayout(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderLinkage>& shaderLinkage)
{
    m_device            = device;
    descriptorSetLayout = std::make_shared<DescriptorSetLayout>(device, shaderLinkage);

    for (const auto& [_, shaderModule] : shaderLinkage->shaderModules)
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
    LOGI("VulkanRHI", "{}\n{}", PtrToHexString(vkPipelineLayout), GetCallStack())
}
Sandbox::PipelineLayout::~PipelineLayout() { Cleanup(); }

void Sandbox::PipelineLayout::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    LOGD("VulkanRHI", "PipelineLayout Cleanup called {}", PtrToHexString(vkPipelineLayout))
    descriptorSetLayout->Cleanup();
    vkDestroyPipelineLayout(m_device->vkDevice, vkPipelineLayout, nullptr);
    m_cleaned = true;
}
