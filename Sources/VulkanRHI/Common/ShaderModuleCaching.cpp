#include "pch.hpp"

#include "ShaderModuleCaching.hpp"

#include "ResourceCaching.hpp"
Sandbox::ShaderModuleCaching::         ShaderModuleCaching(const std::shared_ptr<Device>& inDevice) : m_device(inDevice) {}
std::shared_ptr<Sandbox::ShaderModule> Sandbox::ShaderModuleCaching::GetOrCreateShaderModule(const ShaderSource& glslSource)
{
    uint64_t hash;
    HashParam(hash, glslSource);
    if (shaderModules.contains(hash))
    {
        return shaderModules.at(hash);
    }
    auto newShaderModule = CreateShaderModule(m_device, glslSource);
    shaderModules[hash]  = newShaderModule;
    return newShaderModule;
}
void Sandbox::ShaderModuleCaching::Cleanup()
{
    for (auto& shaderModule : shaderModules)
    {
        shaderModule.second->Cleanup();
    }
    shaderModules.clear();
}
std::shared_ptr<Sandbox::ShaderModule> Sandbox::ShaderModuleCaching::CreateShaderModule(const std::shared_ptr<Device>& device, const ShaderSource& glslSource)
{
    return std::make_shared<ShaderModule>(device, glslSource);
}
