#include "pch.hpp"

#include "PipelineState.hpp"

#include "ShaderLinkage.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"

Sandbox::PipelineState::PipelineState(const std::shared_ptr<ShaderLinkage>& inShaderLinkage, const std::shared_ptr<RenderPass>& inRenderPass,
                                      const std::shared_ptr<VertexInputStateBindingModifier>& inBindingModifier) :
    subpassIndex(0), shaderLinkage(inShaderLinkage), renderPass(inRenderPass), m_vertexInputStateBindingModifier(inBindingModifier)
{
    ReflectVertexInputStateFromShader();
}

Sandbox::PipelineState::PipelineState(const std::shared_ptr<ShaderLinkage>& inShaderLinkage, const std::shared_ptr<RenderPass>& inRenderPass)
{
    subpassIndex  = 0;
    renderPass    = inRenderPass;
    shaderLinkage = inShaderLinkage;
    ReflectVertexInputStateFromShader();
}


Sandbox::PipelineState::PipelineState(const std::shared_ptr<ShaderLinkage>& inShaderLinkage, const std::shared_ptr<RenderPass>& renderPass, uint32_t inSubpassIndex) :
    PipelineState(inShaderLinkage, renderPass)
{
    subpassIndex = inSubpassIndex;
}

void Sandbox::PipelineState::ReflectVertexInputStateFromShader()
{
    auto vertexShader = shaderLinkage->GetShaderModuleByStage(VK_SHADER_STAGE_VERTEX_BIT);
    if (vertexShader != nullptr)
    {
        vertexShader->ReflectVertexInputState(vertexInputState, m_vertexInputStateBindingModifier);
        vertexShader->onShaderRecompile.Bind(
            [this](const std::shared_ptr<ShaderModule>& recompiledShader)
            {
                vertexInputState.attributes.clear();
                vertexInputState.bindings.clear();
                recompiledShader->ReflectVertexInputState(vertexInputState, m_vertexInputStateBindingModifier);
            });
    }
}

Sandbox::PipelineState::PipelineState(const PipelineState& other)
{
    renderPass       = other.renderPass;
    shaderLinkage    = other.shaderLinkage;
    vertexInputState = other.vertexInputState;
    // pipelineLayout   = other.pipelineLayout;
    // pushConstantsInfo  = other.pushConstantsInfo;
    inputAssemblyState = other.inputAssemblyState;
    rasterizationState = other.rasterizationState;
    depthStencilState  = other.depthStencilState;
    multisampleState   = other.multisampleState;
    subpassIndex       = other.subpassIndex;
}

bool Sandbox::PipelineState::operator==(const PipelineState& other) const
{
    return inputAssemblyState == other.inputAssemblyState && vertexInputState == other.vertexInputState && rasterizationState == other.rasterizationState &&
        depthStencilState == other.depthStencilState && shaderLinkage.get() == other.shaderLinkage.get() && renderPass.get() == other.renderPass.get() &&
        subpassIndex == other.subpassIndex;
}
