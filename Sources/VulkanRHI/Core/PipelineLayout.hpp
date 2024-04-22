#pragma once
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class DescriptorSetLayout;
    class ShaderModule;
    class Device;
    class ShaderLinkage;

    class PipelineLayout
    {
    public:
        /**
         * \brief
         * \param shaderLinkage
         */
        PipelineLayout(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderLinkage>& shaderLinkage);

        ~PipelineLayout();

        void Cleanup();

        VkPipelineLayout                     vkPipelineLayout;
        std::shared_ptr<DescriptorSetLayout> descriptorSetLayout;
        std::vector<VkPushConstantRange>     pushConstantRanges;

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
