#pragma once
#include "Standard/Dictionary.hpp"

namespace Sandbox
{
    class ShaderModule;
    class Device;
    class ShaderSource;

    class ShaderModuleCaching
    {
    public:
        ShaderModuleCaching(const std::shared_ptr<Device>& inDevice);

        std::shared_ptr<ShaderModule> GetOrCreateShaderModule(const ShaderSource& glslSource);

        void Cleanup();

    private:
        std::shared_ptr<Device> m_device;

        Dictionary<uint64_t, std::shared_ptr<ShaderModule>> shaderModules;

        std::shared_ptr<ShaderModule> CreateShaderModule(const std::shared_ptr<Device>& device, const ShaderSource& glslSource);
    };
}  // namespace Sandbox
