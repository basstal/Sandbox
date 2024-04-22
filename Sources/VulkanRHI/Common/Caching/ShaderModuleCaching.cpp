#include "pch.hpp"

#include "ShaderModuleCaching.hpp"

#include "FileSystem/Logger.hpp"
#include "Misc/DataBinding.hpp"
#include "ResourceCaching.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
Sandbox::ShaderModuleCaching::ShaderModuleCaching(const std::shared_ptr<Device>& inDevice) : m_device(inDevice) {}

std::shared_ptr<Sandbox::ShaderModule> Sandbox::ShaderModuleCaching::GetOrCreateShaderModule(const std::shared_ptr<ShaderSource>& glslSource, VkShaderStageFlagBits inStage)
{
    // uint64_t hash;
    // HashParam(hash, glslSource);
    if (m_shaderModules.contains(*glslSource))
    {
        // LOGD("VulkanRHI", "reuse shader module from cache hash {}", std::to_string(hash))
        return m_shaderModules.at(*glslSource);
    }
    auto newShaderModule         = CreateShaderModule(m_device, glslSource, inStage);
    m_shaderModules[*glslSource] = newShaderModule;
    return newShaderModule;
    // auto newShaderModule = CreateShaderModule(m_device, glslSource);
    // return newShaderModule;
}

void Sandbox::ShaderModuleCaching::RecompileAll()
{
    Dictionary<ShaderSource, std::shared_ptr<ShaderModule>> newShaderModules;
    for (auto& [_, shaderModule] : m_shaderModules)
    {
        shaderModule->Recompile();
        newShaderModules[*shaderModule->m_shaderSource] = shaderModule;
    }
    m_shaderModules.clear();
    m_shaderModules = newShaderModules;
}

void Sandbox::ShaderModuleCaching::Cleanup()
{
    for (auto& [_, shaderModule] : m_shaderModules)
    {
        shaderModule->Cleanup();
    }
    m_shaderModules.clear();
}

std::shared_ptr<Sandbox::ShaderModule> Sandbox::ShaderModuleCaching::CreateShaderModule(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderSource>& glslSource,
                                                                                        VkShaderStageFlagBits inStage)
{
    return std::make_shared<ShaderModule>(device, glslSource, inStage);
}
