#include "pch.hpp"

#include "StencilRendererSource.hpp"

#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "FileSystem/Directory.hpp"
#include "Misc/Debug.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"
void Sandbox::StencilRendererSource::Prepare(std::shared_ptr<Renderer>& renderer)
{
    WireframeRendererSource::Prepare(renderer);
    m_renderer = renderer;
}
void Sandbox::StencilRendererSource::CreatePipeline(std::shared_ptr<Renderer>& renderer)
{
    WireframeRendererSource::CreatePipeline(renderer);
    VkStencilOpState vkStencilOpState;
    vkStencilOpState.failOp      = VK_STENCIL_OP_KEEP;
    vkStencilOpState.passOp      = VK_STENCIL_OP_REPLACE;
    vkStencilOpState.depthFailOp = VK_STENCIL_OP_REPLACE;
    // 对应 glStencilMask(0xFF);
    vkStencilOpState.writeMask = 0xFF;  // 设置前面模板写入掩码

    // 对应 glStencilFunc(GL_ALWAYS, 1, 0xFF);
    vkStencilOpState.compareOp   = VK_COMPARE_OP_ALWAYS;  // 模板测试总是通过
    vkStencilOpState.compareMask = 0xFF;  // 参与比较的掩码
    vkStencilOpState.reference   = 1;  // 参考值

    VkStencilOpState backOpState;
    backOpState.failOp      = VK_STENCIL_OP_KEEP;
    backOpState.passOp      = VK_STENCIL_OP_REPLACE;
    backOpState.depthFailOp = VK_STENCIL_OP_KEEP;
    backOpState.compareOp   = VK_COMPARE_OP_NEVER;
    backOpState.compareMask = 0xFF;
    backOpState.writeMask   = 0xFF;
    backOpState.reference   = 1;

    // 这里需要对所有固定渲染管线都创建一个对应的 stencil pipeline，先拿到原来的 pipelineState ，加入模板设置
    for (auto& [viewMode, rendererSource] : renderer->rendererSourceMapping)
    {
        // 先只做 lit 的
        if (viewMode != Lit)
        {
            continue;
        }
        // TODO:不需要这样复制构造
        auto pipelineState                                 = std::make_shared<PipelineState>(*rendererSource->pipelineState);
        pipelineState->rasterizationState.polygonMode      = VK_POLYGON_MODE_FILL;
        pipelineState->depthStencilState.stencilTestEnable = VK_TRUE;

        pipelineState->depthStencilState.front = vkStencilOpState;
        pipelineState->depthStencilState.back  = backOpState;
        stencilInputLitPipeline                = renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);
    }
    auto edgeColor                                              = glm::vec3(243.0f / 255, 151.0f / 255, 44.0f / 255);
    auto pipelineStateOutline                                   = std::make_shared<PipelineState>(shaderLinkage, renderer->renderPass);
    pipelineStateOutline->multisampleState.rasterizationSamples = renderer->device->GetMaxUsableSampleCount();
    // if (shaderLinkage[0]->GetUniformBlocks().size() != 2)
    // {
    //     LOGF("Editor", "StencilRendererSource::CreatePipeline: shaderModules[0]->GetUniformBlocks().size() != 2")
    // }
    pipelineStateOutline->depthStencilState.stencilTestEnable = VK_TRUE;
    pipelineStateOutline->depthStencilState.depthTestEnable   = VK_FALSE;

    // 对应 glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    vkStencilOpState.compareOp   = VK_COMPARE_OP_NOT_EQUAL;  // 当模板值不等于参考值时通过
    vkStencilOpState.compareMask = 0xFF;  // 参与比较的掩码
    vkStencilOpState.reference   = 1;  // 参考值
    // 对应 glStencilMask(0x00);
    vkStencilOpState.writeMask                    = 0x00;  // 设置前面模板写入掩码
    pipelineStateOutline->depthStencilState.front = vkStencilOpState;
    pipelineStateOutline->depthStencilState.back  = backOpState;

    stencilOutlinePipeline = renderer->pipelineCaching->GetOrCreatePipeline(pipelineStateOutline);
    if (stencilOutlinePipeline.get() == stencilInputLitPipeline.get())
    {
        LOGF("VulkanRHI", "stencilOutlinePipeline == stencilInputLitPipeline pipelineState hash conflict?")
    }
    // // TODO: 这里代码从基类复制出来的，先临时修正 descriptorSets 资源数量不正确的问题
    // auto     device          = renderer->device;
    // uint32_t frameFlightSize = renderer->maxFramesFlight;
    // descriptorSets.resize(frameFlightSize);
    // for (size_t i = 0; i < frameFlightSize; ++i)
    // {
    //     descriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet(stencilOutlinePipeline->pipelineLayout->descriptorSetLayout, i);
    // }
    // UpdateDescriptorSets(renderer);

    colorUniformBuffer->Update(&edgeColor);
}

void Sandbox::StencilRendererSource::CustomDrawOverlay(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<CommandBuffer>& commandBuffer,
                                                       const std::shared_ptr<DescriptorSet>& descriptorSet, uint32_t frameFlightIndex, uint32_t dynamicOffsets)
{
    // auto boundPipeline = commandBuffer->GetBoundPipeline();
    // 修改缩放再绘制一次
    auto transform = mesh->transform.lock();
    // TODO:已经缩放很大的物体可能选中边界会很小
    transform->scale += 0.05f;
    Tick(m_renderer);
    // LOGD("Engine", "CustomDrawOverlay bind stencilOutlinePipeline {}", PtrToHexString(stencilOutlinePipeline->vkPipeline))
    commandBuffer->BindPipeline(stencilOutlinePipeline);
    // LOGD("Engine", "CustomDrawOverlay BindDescriptorSet {}", PtrToHexString(stencilOutlinePipeline->pipelineLayout->vkPipelineLayout))
    commandBuffer->BindDescriptorSet(stencilOutlinePipeline->pipelineLayout, descriptorSets[frameFlightIndex], {dynamicOffsets});
    commandBuffer->BindVertexBuffers(mesh->vertexBuffer);
    commandBuffer->BindIndexBuffer(mesh->indexBuffer);
    commandBuffer->DrawIndexed(mesh->Indices());

    transform->scale -= 0.05f;
    // commandBuffer->BindPipeline(boundPipeline);
}
void Sandbox::StencilRendererSource::CustomDrawMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<CommandBuffer>& commandBuffer,
                                                    const std::shared_ptr<DescriptorSet>& descriptorSet, uint32_t frameFlightIndex, uint32_t dynamicOffsets)
{
    WireframeRendererSource::CustomDrawMesh(mesh, commandBuffer, descriptorSet, frameFlightIndex, dynamicOffsets);
    // auto boundPipeline = commandBuffer->GetBoundPipeline();
    commandBuffer->BindPipeline(stencilInputLitPipeline);
    // LOGD("Engine", "CustomDrawMesh BindDescriptorSet {}", PtrToHexString(stencilOutlinePipeline->pipelineLayout->vkPipelineLayout))
    commandBuffer->BindDescriptorSet(stencilInputLitPipeline->pipelineLayout, descriptorSet, {dynamicOffsets});
    commandBuffer->BindVertexBuffers(mesh->vertexBuffer);
    commandBuffer->BindIndexBuffer(mesh->indexBuffer);
    commandBuffer->DrawIndexed(mesh->Indices());
    // commandBuffer->BindPipeline(boundPipeline);
}

void Sandbox::StencilRendererSource::Cleanup(){
    stencilInputLitPipeline->Cleanup();
    stencilOutlinePipeline->Cleanup();
WireframeRendererSource::Cleanup();
    
}
