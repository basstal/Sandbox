#include "TransformGizmo.hpp"

#include <vector>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include "SimpleVertex.hpp"
#include "Infrastructures/FileSystemBase.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/CommandResource.hpp"
#include "Rendering/Components/Pipeline.hpp"

const float M_PI = 3.1415926f;
const int segments = 32;

TransformGizmo::TransformGizmo(const std::shared_ptr<GameObject> target, const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource,
                               const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<DescriptorResource> descriptorResource)
{
	m_device = device;
	referenceGameObject = target;

	PrepareDrawData(device, commandResource, pipeline, descriptorResource);
}

TransformGizmo::~TransformGizmo()
{
	Cleanup();
}

void TransformGizmo::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	if (vkPipeline != nullptr)
	{
		vkDestroyPipeline(m_device->vkDevice, vkPipeline, nullptr);
	}
	if (buffer != nullptr)
	{
		buffer->Cleanup();
	}
	m_cleaned = true;
}

std::vector<SimpleVertex> TransformGizmo::GenerateArrowData(glm::vec3 color)
{
	std::vector<SimpleVertex> vertices;
	float deltaAngle = 2.0f * M_PI / segments;
	for (int i = 0; i < segments; ++i)
	{
		float angle = static_cast<float>(i) * deltaAngle;
		float nextAngle = static_cast<float>(i + 1) * deltaAngle;

		// 底部圆的顶点
		// glm::vec3 bottom0(0, 0, 0);
		glm::vec3 bottom1(radiusCylinder * cos(angle), radiusCylinder * sin(angle), 0);
		glm::vec3 bottom2(radiusCylinder * cos(nextAngle), radiusCylinder * sin(nextAngle), 0);

		// 顶部圆的顶点
		// glm::vec3 top0(0, 0, heightCylinder);
		glm::vec3 top1(radiusCylinder * cos(angle), radiusCylinder * sin(angle), heightCylinder);
		glm::vec3 top2(radiusCylinder * cos(nextAngle), radiusCylinder * sin(nextAngle), heightCylinder);

		// 添加侧面的顶点
		vertices.push_back({bottom1, color});
		vertices.push_back({bottom2, color});
		vertices.push_back({top1, color});

		vertices.push_back({bottom2, color});
		vertices.push_back({top2, color});
		vertices.push_back({top1, color});
	}

	glm::vec3 coneTip(0, 0, heightCylinder + heightCone); // 锥体顶点位于圆柱体顶部之上
	for (int i = 0; i < segments; ++i)
	{
		float angle = static_cast<float>(i) * deltaAngle;
		float nextAngle = static_cast<float>(i + 1) * deltaAngle;

		// 锥体基底圆的顶点
		glm::vec3 base1(radiusCone * cos(angle), radiusCone * sin(angle), heightCylinder);
		glm::vec3 base2(radiusCone * cos(nextAngle), radiusCone * sin(nextAngle), heightCylinder);

		// 添加锥体侧面的顶点
		vertices.push_back({coneTip, color});
		vertices.push_back({base1, color});
		vertices.push_back({base2, color});
	}
	return vertices;
}

glm::vec3 RotateVectorByQuaternion(const glm::vec3& v, const glm::quat& q)
{
	// 将向量v转换为四元数，实部为0
	glm::quat vQuat(0.0f, v.x, v.y, v.z);

	// 计算旋转：qvq*
	glm::quat qConj = glm::conjugate(q); // 使用共轭代替逆
	glm::quat rotatedQuat = q * vQuat * qConj;

	// 返回旋转后的向量部分
	return glm::vec3(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
}

void TransformGizmo::PrepareDrawData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline,
                                     const std::shared_ptr<DescriptorResource>& descriptorResource)
{
	auto arrowX = GenerateArrowData(glm::vec3(1.0f, 0.0f, 0.0f));
	// 对 arrowX 中每个点应用绕 y 轴 90 度旋转
	glm::quat rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (auto& vertex : arrowX)
	{
		vertex.position = RotateVectorByQuaternion(vertex.position, rotation);
	}
	auto arrowY = GenerateArrowData(glm::vec3(0.0f, 1.0f, 0.0f));
	// 对 arrowY 中每个点应用绕 x 轴 -90 度旋转
	rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (auto& vertex : arrowY)
	{
		vertex.position = RotateVectorByQuaternion(vertex.position, rotation);
	}
	auto arrowZ = GenerateArrowData(glm::vec3(0.0f, 0.0f, 1.0f));
	gizmoData.insert(gizmoData.end(), arrowX.begin(), arrowX.end());
	gizmoData.insert(gizmoData.end(), arrowY.begin(), arrowY.end());
	gizmoData.insert(gizmoData.end(), arrowZ.begin(), arrowZ.end());


	auto vkDevice = device->vkDevice;
	VkDeviceSize bufferSize = sizeof(SimpleVertex) * gizmoData.size();

	buffer = std::make_shared<Buffer>(device, bufferSize,
	                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Buffer bufferStaging(device, bufferSize,
	                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// 将顶点数据复制到暂存缓冲
	void* data;
	vkMapMemory(vkDevice, bufferStaging.vkDeviceMemory, 0, bufferSize, 0, &data);
	memcpy(data, gizmoData.data(), bufferSize);
	vkUnmapMemory(vkDevice, bufferStaging.vkDeviceMemory);

	// 复制暂存缓冲到顶点缓冲
	VkCommandBuffer commandBuffer = commandResource->BeginSingleTimeCommands();
	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, bufferStaging.vkBuffer, buffer->vkBuffer, 1, &copyRegion);
	commandResource->EndSingleTimeCommands(commandBuffer);

	std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	auto nonSolidVertex = FileSystemBase::readFile((binariesDir / "Shaders/FillModeNonSolidTransformGizmo_vert.spv").string());
	auto nonSolidFrag = FileSystemBase::readFile((binariesDir / "Shaders/FillModeNonSolidTransformGizmo_frag.spv").string());
	VkShaderModule vertShaderModule = pipeline->CreateShaderModule(nonSolidVertex);
	VkShaderModule fragShaderModule = pipeline->CreateShaderModule(nonSolidFrag);

	// 创建管线
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription = getBindingDescription();
	auto attributeDescriptions = getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_FALSE;
	depthStencil.depthWriteEnable = VK_FALSE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional
	vkPipeline = pipeline->CreatePipeline(vertShaderModule, fragShaderModule, false, vertexInputInfo, inputAssembly, depthStencil);
	CreatePushConstantPipelineLayout(descriptorResource);
}

void TransformGizmo::CreatePushConstantPipelineLayout(const std::shared_ptr<DescriptorResource>& descriptorResource)
{
	VkPushConstantRange pushConstantRange;
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // 假设模型矩阵在顶点着色器中使用
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(glm::mat4); // mat4的大小

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorResource->vkDescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(m_device->vkDevice, &pipelineLayoutInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}
void TransformGizmo::Draw(const std::shared_ptr<Device>& device, const VkCommandBuffer& currentCommandBuffer, const std::shared_ptr<Pipeline>& pipeline,
                          const std::shared_ptr<DescriptorResource>& descriptorResource, uint32_t currentFrame)
{
	vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);


	VkBuffer vertexBuffers[] = {buffer->vkBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &descriptorResource->vkDescriptorSets[currentFrame], 0, nullptr);
	vkCmdPushConstants(
		currentCommandBuffer, // 当前的命令缓冲区
		vkPipelineLayout, // 使用的管线布局
		VK_SHADER_STAGE_VERTEX_BIT, // 着色器阶段
		0, // 偏移量
		sizeof(glm::mat4), // 数据大小
		&modelMatrix // 指向数据的指针
	);
	vkCmdDraw(currentCommandBuffer, static_cast<uint32_t>(gizmoData.size()), 1, 0, 0);
}


void TransformGizmo::AdjustGizmoSize(const std::shared_ptr<Camera>& camera)
{
	float distance = glm::length(referenceGameObject->transform->position - camera->position);
	float baseSize = .25f; // 基础尺寸
	float scaleFactor = distance * 0.25f; // 根据距离计算缩放因子
	modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor * baseSize));
}
