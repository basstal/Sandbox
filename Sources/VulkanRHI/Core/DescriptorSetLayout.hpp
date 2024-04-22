﻿#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Sandbox
{
    class Device;
    class ShaderModule;
    class ShaderLinkage;
    
    class DescriptorSetLayout
    {
    public:
        DescriptorSetLayout(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderLinkage>& shaderLinkage);

        ~DescriptorSetLayout();

        void Cleanup();

        VkDescriptorSetLayout vkDescriptorSetLayout;

        // TODO: 好像没什么用了
        std::map<std::string, uint32_t> nameToBinding;

        bool TryGetLayoutBinding(uint32_t bindingIndex, VkDescriptorSetLayoutBinding& out);

    private:
        std::map<uint32_t, VkDescriptorSetLayoutBinding> m_bindingToLayoutBinding;
        std::shared_ptr<Device>                          m_device;
        bool                                             m_cleaned = false;
    };
}
