#pragma once
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "Standard/Dictionary.hpp"

namespace Sandbox
{
    class ShaderModule;
    class Device;

    class ShaderModuleCaching
    {
    public:
        ShaderModuleCaching(const std::shared_ptr<Device>& inDevice);

        std::shared_ptr<ShaderModule> GetOrCreateShaderModule(const std::shared_ptr<ShaderSource>& glslSource, VkShaderStageFlagBits inStage);


        void RecompileAll();
        
        void Cleanup();

    private:
        std::shared_ptr<Device> m_device;

        Dictionary<ShaderSource, std::shared_ptr<ShaderModule>> m_shaderModules;

        std::shared_ptr<ShaderModule> CreateShaderModule(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderSource>& glslSource, VkShaderStageFlagBits inStage);
    };
}  // namespace Sandbox
