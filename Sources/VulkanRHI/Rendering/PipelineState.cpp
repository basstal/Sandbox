#include "PipelineState.hpp"

#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"

Sandbox::PipelineState::PipelineState(const std::vector<std::shared_ptr<ShaderModule>>& inShaderModules, const std::shared_ptr<RenderPass>& inRenderPass,
                                      const std::shared_ptr<PipelineLayout>& inPipelineLayout)
{
    renderPass = inRenderPass;
    shaderModules = inShaderModules;
    for (const auto& shaderModule : shaderModules)
    {
        if (shaderModule->vkShaderStage == VK_SHADER_STAGE_VERTEX_BIT)
        {
            shaderModule->ReflectVertexInputState(vertexInputState);
        }
    }
    pipelineLayout = inPipelineLayout;
    if (!pipelineLayout->pushConstantRanges.empty())
    {
        pushConstantsInfo = {pipelineLayout->pushConstantRanges[0].size, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT};
    }
}
