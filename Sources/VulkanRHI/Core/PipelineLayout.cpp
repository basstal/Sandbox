#include "pch.hpp"

#include "PipelineLayout.hpp"

#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "Misc/TypeCasting.hpp"
#include "ShaderModule.hpp"
#include "VulkanRHI/Rendering/DescriptorSetsPreset.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"

Sandbox::PipelineLayout::PipelineLayout(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderLinkage>& shaderLinkage,
                                        const std::shared_ptr<DescriptorSetsPreset>& descriptorSetsPreset)
{
    m_device = device;
    if (descriptorSetsPreset == nullptr)
    {
        descriptorSetLayouts.push_back(std::make_shared<DescriptorSetLayout>(device, shaderLinkage));
    }
    else
    {
        if (descriptorSetsPreset->count <= 0)
        {
            LOGF("VulkanRHI", "descriptorSetsPreset->count <= 0!")
        }
        for (size_t i = 0; i < descriptorSetsPreset->count; ++i)
        {
            descriptorSetLayouts.push_back(std::make_shared<DescriptorSetLayout>(device, shaderLinkage, descriptorSetsPreset->operator[](i)));
        }
    }

    for (const auto& [_, shaderModule] : shaderLinkage->shaderModules)
    {
        shaderModule->ReflectPushConstantRanges(pushConstantRanges);
    }
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;

    vkDescriptorSetLayouts.reserve(descriptorSetLayouts.size());
    for (const auto& descriptorSetLayout : descriptorSetLayouts)
    {
        vkDescriptorSetLayouts.push_back(descriptorSetLayout->vkDescriptorSetLayout);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = ToUInt32(vkDescriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts            = vkDescriptorSetLayouts.data();
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
    for (const auto& descriptorSetLayoutRef : descriptorSetLayouts)
    {
        descriptorSetLayoutRef->Cleanup();
    }
    vkDestroyPipelineLayout(m_device->vkDevice, vkPipelineLayout, nullptr);
    m_cleaned = true;
}
