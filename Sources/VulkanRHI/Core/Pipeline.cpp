#include "pch.hpp"

#include "Pipeline.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/TypeCasting.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "ShaderModule.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"

// // TODO: remove this method
// Sandbox::Pipeline::Pipeline(const std::shared_ptr<Device>& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules,
//                             const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<PipelineLayout>& pipelineLayout, VkPrimitiveTopology primitiveTopology,
//                             VkPolygonMode polygonMode) :
//     Pipeline(device, CreatePipelineState(shaderModules, renderPass, pipelineLayout, primitiveTopology, polygonMode))
// {
// }

Sandbox::Pipeline::Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<PipelineState>& pipelineState) :
    shaderLinkage(pipelineState->shaderLinkage), m_pipelineState(pipelineState), m_device(device)  // pipelineLayout(pipelineState->pipelineLayout), m_device(device)
{
    pipelineLayout = std::make_shared<PipelineLayout>(device, pipelineState->shaderLinkage, pipelineState->descriptorSetsPreset);
    CreatePipeline(pipelineState);
}

// // TODO: remove this method
// std::shared_ptr<Sandbox::PipelineState> Sandbox::Pipeline::CreatePipelineState(const std::vector<std::shared_ptr<ShaderModule>>& shaderModules,
//                                                                                const std::shared_ptr<RenderPass>&                renderPass,
//                                                                                const std::shared_ptr<PipelineLayout>& inPipelineLayout, VkPrimitiveTopology primitiveTopology,
//                                                                                VkPolygonMode polygonMode)
// {
//     auto createdPipelineState                            = std::make_shared<PipelineState>(shaderModules, renderPass, inPipelineLayout);
//     createdPipelineState->inputAssemblyState.topology    = primitiveTopology;
//     createdPipelineState->rasterizationState.polygonMode = polygonMode;
//     return createdPipelineState;
// }

void Sandbox::Pipeline::CreatePipeline(const std::shared_ptr<PipelineState>& inPipelineState)
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = inPipelineState->inputAssemblyState.topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = inPipelineState->rasterizationState.polygonMode;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = inPipelineState->rasterizationState.cullMode;
    rasterizer.frontFace               = inPipelineState->rasterizationState.frontFace;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
    rasterizer.depthBiasClamp          = 0.0f;  // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f;  // Optional

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = ToUInt32(inPipelineState->vertexInputState.bindings.size());
    vertexInputInfo.pVertexBindingDescriptions      = inPipelineState->vertexInputState.bindings.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(inPipelineState->vertexInputState.attributes.size());
    vertexInputInfo.pVertexAttributeDescriptions    = inPipelineState->vertexInputState.attributes.data();


    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    for (const auto& [_, shaderModule] : inPipelineState->shaderLinkage->shaderModules)
    {
        VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo{};
        vkPipelineShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkPipelineShaderStageCreateInfo.stage  = shaderModule->vkShaderStage;
        vkPipelineShaderStageCreateInfo.module = shaderModule->vkShaderModule;
        vkPipelineShaderStageCreateInfo.pName  = "main";
        shaderStages.push_back(vkPipelineShaderStageCreateInfo);
    }

    // use viewport and scissor dynamically
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = inPipelineState->multisampleState.sampleShadingEnable;
    multisampling.minSampleShading      = inPipelineState->multisampleState.minSampleShading;
    multisampling.rasterizationSamples  = m_device->GetUsableSampleCount(inPipelineState->multisampleState.rasterizationSamples);
    multisampling.pSampleMask           = nullptr;  // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
    multisampling.alphaToOneEnable      = VK_FALSE;  // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable         = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;  // Optional
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;  // Optional
    colorBlending.blendConstants[1] = 0.0f;  // Optional
    colorBlending.blendConstants[2] = 0.0f;  // Optional
    colorBlending.blendConstants[3] = 0.0f;  // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = inPipelineState->depthStencilState.depthTestEnable;
    depthStencil.depthWriteEnable      = inPipelineState->depthStencilState.depthWriteEnable;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = inPipelineState->depthStencilState.stencilTestEnable;
    depthStencil.minDepthBounds        = 0.0f;  // Optional
    depthStencil.maxDepthBounds        = 1.0f;  // Optional
    depthStencil.front                 = inPipelineState->depthStencilState.front;
    depthStencil.back                  = inPipelineState->depthStencilState.back;


    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencil;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = pipelineLayout->vkPipelineLayout;
    pipelineInfo.renderPass          = inPipelineState->renderPass->vkRenderPass;
    pipelineInfo.subpass             = inPipelineState->subpassIndex;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;  // Optional
    pipelineInfo.basePipelineIndex   = -1;  // Optional

    if (vkCreateGraphicsPipelines(m_device->vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkPipeline) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create graphics pipeline!");
    }
}

Sandbox::Pipeline::~Pipeline() { Cleanup(); }

void Sandbox::Pipeline::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyPipeline(m_device->vkDevice, vkPipeline, nullptr);
    pipelineLayout->Cleanup();
    m_cleaned = true;
}
void Sandbox::Pipeline::Reload()
{
    if (vkPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device->vkDevice, vkPipeline, nullptr);
        pipelineLayout->Cleanup();
    }
    pipelineLayout = std::make_shared<PipelineLayout>(m_device, m_pipelineState->shaderLinkage, m_pipelineState->descriptorSetsPreset);
    CreatePipeline(m_pipelineState);
}
