#pragma once
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class DescriptorSetLayout;
    class ShaderModule;
    class Device;

    class PipelineLayout
    {
    public:
        /**
         * \brief
         * \param device
         * \param shaderModules
         * \param dynamicBindings 对应 shader 中 binding 为 dynamic 的 uniform buffer 资源
         */
        PipelineLayout(const std::shared_ptr<Device>& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules, const std::vector<uint32_t>& dynamicBindings);

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
