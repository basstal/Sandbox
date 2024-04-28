#pragma once
#include <memory>

#include "Misc/Event.hpp"
#include "Standard/Dictionary.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"

namespace Sandbox
{
    class Renderer;

    class ShaderLinkage : public std::enable_shared_from_this<ShaderLinkage>
    {
    public:
        ShaderLinkage();


        ~ShaderLinkage();

        // void Cleanup();

        std::shared_ptr<Sandbox::ShaderModule> LinkShaderModule(const std::shared_ptr<Renderer>& inRenderer, VkShaderStageFlagBits stage,
                                                                const std::shared_ptr<ShaderSource>& shaderSource);
        std::shared_ptr<ShaderModule>          GetShaderModuleByStage(VkShaderStageFlagBits stage);


        Dictionary<VkShaderStageFlagBits, std::shared_ptr<ShaderModule>> shaderModules;

        // Event<const std::shared_ptr<ShaderLinkage>> onReload;

    private:
        // bool m_cleaned = false;
    };
}  // namespace Sandbox


namespace std
{
    template <>
    struct hash<Sandbox::ShaderLinkage>
    {
        size_t operator()(const Sandbox::ShaderLinkage& shaderLinkage) const noexcept
        {
            size_t result = 0;
            for (auto& [_, shaderModule] : shaderLinkage.shaderModules)
            {
                Sandbox::HashCombined(result, shaderModule->vkShaderModule);
                Sandbox::HashCombined(result, shaderModule->vkShaderStage);
            }
            return result;
        }
    };
}  // namespace std
