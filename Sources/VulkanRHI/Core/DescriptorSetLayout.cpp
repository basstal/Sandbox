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
        shaderModule->ReflectDescriptorSetLayoutBindings(m_bindingToLayoutBinding);
    }
    // 将 m_bindingToLayoutBinding 的 value 填充到 vkDescriptorSetLayoutBindings
    for (auto [_, layoutBinding] : m_bindingToLayoutBinding)
    {
        vkDescriptorSetLayoutBindings.push_back(layoutBinding);
    }
    CreateDescriptorSetLayout(vkDescriptorSetLayoutBindings);
}

Sandbox::DescriptorSetLayout::DescriptorSetLayout(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderLinkage>& shaderLinkage,
                                                  const std::set<uint32_t>& filteredBindingIndex)
{
    m_device = device;
    std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
    for (const auto& [_, shaderModule] : shaderLinkage->shaderModules)
    {
        shaderModule->ReflectDescriptorSetLayoutBindings(m_bindingToLayoutBinding);
    }
    // 将 m_bindingToLayoutBinding 的 value 填充到 vkDescriptorSetLayoutBindings
    for (auto [binding, layoutBinding] : m_bindingToLayoutBinding)
    {
        if (filteredBindingIndex.contains(binding))
        {
            vkDescriptorSetLayoutBindings.push_back(layoutBinding);
        }
    }
    CreateDescriptorSetLayout(vkDescriptorSetLayoutBindings);
}

Sandbox::DescriptorSetLayout::~DescriptorSetLayout() { Cleanup(); }


void Sandbox::DescriptorSetLayout::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings)
{
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
