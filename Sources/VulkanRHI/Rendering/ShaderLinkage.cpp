#include "pch.hpp"

#include "ShaderLinkage.hpp"

#include "VulkanRHI/Common/Caching/ShaderModuleCaching.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"

Sandbox::ShaderLinkage::ShaderLinkage() {}

Sandbox::ShaderLinkage::~ShaderLinkage() { }//Cleanup(); }

// void Sandbox::ShaderLinkage::Cleanup()
// {
//     if (m_cleaned)
//     {
//         return;
//     }
//     // for (auto& [_, shaderModule] : shaderModules)
//     // {
//     //     shaderModule->Cleanup();
//     // }
//     m_cleaned = true;
// }

std::shared_ptr<Sandbox::ShaderModule> Sandbox::ShaderLinkage::LinkShaderModule(const std::shared_ptr<Renderer>& inRenderer, VkShaderStageFlagBits stage,
                                                                                const std::shared_ptr<ShaderSource>& shaderSource)
{
    auto shaderModule    = inRenderer->shaderModuleCaching->GetOrCreateShaderModule(shaderSource, stage);
    shaderModules[stage] = shaderModule;
    // // TODO:在这里绑定会绑定多个可能不合适，不过先测着
    // shaderModule->onShaderRecompile.Bind([this](const std::shared_ptr<ShaderModule>& _) { onReload.Trigger(shared_from_this()); });
    return shaderModule;
}
std::shared_ptr<Sandbox::ShaderModule> Sandbox::ShaderLinkage::GetShaderModuleByStage(VkShaderStageFlagBits stage)
{
    if (shaderModules.contains(stage))
    {
        return shaderModules[stage];
    }
    return nullptr;
}
