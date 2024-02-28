#include "Pipeline.hpp"

#include <filesystem>
#include <stdexcept>

#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Rendering/Components/DescriptorResource.hpp"
#include "Rendering/Components/RenderPass.hpp"
#include "GameCore/Vertex.hpp"
#include "Rendering/RendererSettings.hpp"
#include "Rendering/Objects/Shader.hpp"


Pipeline::Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<Shader>& shader, const std::shared_ptr<RenderPass>& renderPass, VkPrimitiveTopology primitiveTopology,
                   VkPolygonMode polygonMode)
{
	m_device = device;
	m_renderPass = renderPass;
	m_shader = shader;
	descriptorResource = shader->CreateDescriptorResource();
	descriptorResource->CreateDescriptorSets();
	vkPipelineLayout = CreatePipelineLayout(descriptorResource, descriptorResource->vkDescriptorSetLayout);

	VkShaderModule vertexShaderModule = m_shader->GetShaderModuleStage(VK_SHADER_STAGE_VERTEX_BIT);
	VkShaderModule fragmentShaderModule = m_shader->GetShaderModuleStage(VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(descriptorResource->vkVertexInputAttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &descriptorResource->vkVertexInputBindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = descriptorResource->vkVertexInputAttributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = primitiveTopology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexShaderModule;
	vertShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentShaderModule;
	fragShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	// use viewport and scissor dynamically
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;
	multisampling.minSampleShading = .2f; // Optional
	multisampling.rasterizationSamples = m_renderPass->enableMSAA ? m_device->msaaSamples : VK_SAMPLE_COUNT_1_BIT;
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = renderPass->enableDepth ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = renderPass->enableDepth ? VK_TRUE : VK_FALSE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = vkPipelineLayout;
	pipelineInfo.renderPass = renderPass->vkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (vkCreateGraphicsPipelines(m_device->vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkPipeline) != VK_SUCCESS)
	{
		Logger::Fatal("failed to create graphics pipeline!");
	}

	std::shared_ptr<TDataBinding<std::shared_ptr<RendererSettings>>> settingsBinding = DataBinding::Get<std::shared_ptr<RendererSettings>>("Rendering/Settings");
	settingsBinding->BindMember<Pipeline, &Pipeline::ApplySettings>(this);
}
Pipeline::~Pipeline()
{
	// Cleanup();
}

VkPipelineLayout Pipeline::CreatePipelineLayout(const std::shared_ptr<DescriptorResource>& inDescriptorResource, VkDescriptorSetLayout descriptorSetLayout)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(inDescriptorResource->vkPushConstantRanges.size());
	pipelineLayoutInfo.pPushConstantRanges = inDescriptorResource->vkPushConstantRanges.data();

	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(m_device->vkDevice, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		Logger::Fatal("failed to create pipeline layout!");
	}
	return layout;
}

void Pipeline::ApplySettings(std::shared_ptr<RendererSettings> settings)
{
	m_fillModeNonSolid = settings->settingsConfig.ViewMode == Wireframe;
}

void Pipeline::CreatePipeline(const std::vector<char>& vertexShader, const std::vector<char>& fragmentShader)
{
	Logger::Fatal("not implemented");
	// VkShaderModule vertShaderModule = CreateShaderModule(m_device, vertexShader);
	// VkShaderModule fragShaderModule = CreateShaderModule(m_device, fragmentShader);
	// auto vkPipeline = CreatePipeline(vertShaderModule, fragShaderModule, false);
	// vkPipelines.push_back(vkPipeline);
	// vkDestroyShaderModule(m_device->vkDevice, fragShaderModule, nullptr);
	// vkDestroyShaderModule(m_device->vkDevice, vertShaderModule, nullptr);
}



void Pipeline::CreateFillModeNonSolidPipeline()
{
	std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	auto nonSolidVertex = FileSystemBase::readFile((binariesDir / "Shaders/FillModeNonSolid_vert.spv").string());
	auto nonSolidFrag = FileSystemBase::readFile((binariesDir / "Shaders/FillModeNonSolid_frag.spv").string());
	VkShaderModule vertShaderModule = CreateShaderModule(m_device, nonSolidVertex);
	VkShaderModule fragShaderModule = CreateShaderModule(m_device, nonSolidFrag);
	nonSolidPipeline = CreatePipeline(vertShaderModule, fragShaderModule, true);
	vkDestroyShaderModule(m_device->vkDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device->vkDevice, vertShaderModule, nullptr);
}

VkPipeline Pipeline::CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid)
{
	Logger::Fatal("not implemented");
	// VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	// vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// auto bindingDescription = Vertex::GetBindingDescription();
	// auto attributeDescriptions = Vertex::GetAttributeDescriptions();
	//
	// vertexInputInfo.vertexBindingDescriptionCount = 1;
	// vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	// vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	// vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	//
	// VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	// inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	// inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	// inputAssembly.primitiveRestartEnable = VK_FALSE;
	//
	//
	// return CreatePipeline(vertShaderModule, fragShaderModule, fillModeNonSolid, vertexInputInfo, inputAssembly);
    return nullptr;
}

VkPipeline Pipeline::CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
                                    const VkPipelineInputAssemblyStateCreateInfo& inputAssembly)
{
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional
	return CreatePipeline(vertShaderModule, fragShaderModule, fillModeNonSolid, vertexInputInfo, inputAssembly, depthStencil, vkPipelineLayout, m_renderPass->vkRenderPass, true);
}

VkPipeline Pipeline::CreatePipeline(const VkShaderModule& vertShaderModule, const VkShaderModule& fragShaderModule, bool fillModeNonSolid, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
                                    const VkPipelineInputAssemblyStateCreateInfo& inputAssembly, const VkPipelineDepthStencilStateCreateInfo& depthStencil, const VkPipelineLayout& inVkPipelineLayout,
                                    const VkRenderPass& inVkRenderPass, bool useMultiSampling)
{
	VkPipeline vkPipeline;
	// use viewport and scissor dynamically
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = fillModeNonSolid ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;
	multisampling.minSampleShading = .2f; // Optional
	multisampling.rasterizationSamples = m_renderPass->enableMSAA ? m_device->msaaSamples : VK_SAMPLE_COUNT_1_BIT;
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = inVkPipelineLayout;
	pipelineInfo.renderPass = inVkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	pipelineInfo.pDepthStencilState = &depthStencil;
	if (vkCreateGraphicsPipelines(m_device->vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkPipeline) != VK_SUCCESS)
	{
		Logger::Fatal("failed to create graphics pipeline!");
	}
	return vkPipeline;
}

void Pipeline::Cleanup()
{
	if (!m_cleaned)
	{
		vkDestroyPipeline(m_device->vkDevice, vkPipeline, nullptr);
		vkDestroyPipelineLayout(m_device->vkDevice, vkPipelineLayout, nullptr);
		m_cleaned = true;
	}
}

VkPipeline Pipeline::GraphicsPipeline()
{
	if (m_fillModeNonSolid)
	{
		return nonSolidPipeline;
	}
	return vkPipeline;
}


VkShaderModule Pipeline::CreateShaderModule(const std::shared_ptr<Device>& device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device->vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		Logger::Fatal("failed to create shader module!");
	}
	return shaderModule;
}
