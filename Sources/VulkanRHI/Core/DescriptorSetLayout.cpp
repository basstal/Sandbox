#include "pch.hpp"

#include "DescriptorSetLayout.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "ShaderModule.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"

Sandbox::DescriptorSetLayout::DescriptorSetLayout(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderLinkage>& shaderLinkage)
{
    m_device = device;
    std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
    for (const auto& [_, shaderModule] : shaderLinkage->shaderModules)
    {
        shaderModule->ReflectDescriptorSetLayoutBindings(vkDescriptorSetLayoutBindings, nameToBinding, m_bindingToLayoutBinding);
    }
    // for (auto& vkDescriptorSetLayoutBinding : vkDescriptorSetLayoutBindings)
    // {
    //     // 查找 vkDescriptorSetLayoutBinding.binding 是否在 dynamicBindings 中
    //     auto it = std::find(dynamicBindings.begin(), dynamicBindings.end(), vkDescriptorSetLayoutBinding.binding);
    //     if (it != dynamicBindings.end())
    //     {
    //         vkDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    //     }
    // }
    // for (auto& dynamicBinding : dynamicBindings)
    // {
    //     m_bindingToLayoutBinding[dynamicBinding].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    // }
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings    = vkDescriptorSetLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(m_device->vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create descriptor set layout!");
    }

    LOGI("VulkanRHI", "{}\n{}", PtrToHexString(vkDescriptorSetLayout), GetCallStack())
}
Sandbox::DescriptorSetLayout::~DescriptorSetLayout() { Cleanup(); }

void Sandbox::DescriptorSetLayout::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    LOGD("VulkanRHI", "DescriptorSetLayout Cleanup called for {}", PtrToHexString(vkDescriptorSetLayout))
    
    vkDestroyDescriptorSetLayout(m_device->vkDevice, vkDescriptorSetLayout, nullptr);
    m_cleaned = true;
}

bool Sandbox::DescriptorSetLayout::TryGetLayoutBinding(uint32_t bindingIndex, VkDescriptorSetLayoutBinding& out)
{
    auto it = m_bindingToLayoutBinding.find(bindingIndex);
    if (it == m_bindingToLayoutBinding.end())
    {
        return false;
    }
    out = it->second;
    return true;
}
