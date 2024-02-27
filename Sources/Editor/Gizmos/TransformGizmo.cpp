#include "TransformGizmo.hpp"

#include <iostream>
#include <vector>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include "SimpleVertex.hpp"
#include "Editor/ApplicationEditor.hpp"
#include "Infrastructures/CollisionDetect.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/FileSystem/File.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Infrastructures/Math/Ray.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/CommandResource.hpp"
#include "Rendering/Components/Pipeline.hpp"

const float M_PI = 3.1415926f;
const int SEGMENTS = 32;
static bool isGizmoActived = false;
static GLFWcursorposfun lastCallback = nullptr;
const float BASE_SIZE = .25f; // 基础尺寸
const float DISTANCE_SIZE_FACTOR = 0.25f;

void TransformGizmo::ApplyGizmoMovement(GLFWwindow* window, bool active)
{
	if (isGizmoActived == active)
	{
		return;
	}
	if (!isGizmoActived)
	{
		lastCallback = glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
		{
			std::shared_ptr<TDataBinding<std::shared_ptr<ApplicationEditor>>> applicationEditorDataBinding = std::dynamic_pointer_cast<TDataBinding<std::shared_ptr<ApplicationEditor>>>(
				DataBinding::Get("ApplicationEditor"));
			std::shared_ptr<ApplicationEditor> innerApplicationEditor = applicationEditorDataBinding->GetData();
			innerApplicationEditor->transformGizmo->UpdateGizmoAndObjectPosition(window, innerApplicationEditor->editorCamera);
		});
	}
	else
	{
		glfwSetCursorPosCallback(window, lastCallback);
	}
	isGizmoActived = active;
}

TransformGizmo::TransformGizmo(const std::shared_ptr<GameObject> target, const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource,
                               const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<DescriptorResource> descriptorResource, const std::shared_ptr<RenderPass>& renderPass)
{
	m_device = device;
	referenceGameObject = target;

	PrepareDrawData(device, commandResource, descriptorResource, renderPass);
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
	float deltaAngle = 2.0f * M_PI / SEGMENTS;
	for (int i = 0; i < SEGMENTS; ++i)
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
	for (int i = 0; i < SEGMENTS; ++i)
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

void TransformGizmo::PrepareDrawData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource,
                                     const std::shared_ptr<DescriptorResource>& descriptorResource, const std::shared_ptr<RenderPass>& renderPass)
{
	modelMatrix = glm::mat4(1.0f);
	arrowX = GenerateArrowData(glm::vec3(1.0f, 0.0f, 0.0f));
	// 对 arrowX 中每个点应用绕 y 轴 90 度旋转
	glm::quat rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (auto& vertex : arrowX)
	{
		vertex.position = RotateVectorByQuaternion(vertex.position, rotation);
	}
	arrowY = GenerateArrowData(glm::vec3(0.0f, 1.0f, 0.0f));
	// 对 arrowY 中每个点应用绕 x 轴 -90 度旋转
	rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (auto& vertex : arrowY)
	{
		vertex.position = RotateVectorByQuaternion(vertex.position, rotation);
	}
	arrowZ = GenerateArrowData(glm::vec3(0.0f, 0.0f, 1.0f));
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

	std::filesystem::path sourceDir = FileSystemBase::getSourceDir();
	auto shader = std::make_shared<Shader>(m_device);
	shader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/Gizmo.vert").string()), "", VK_SHADER_STAGE_VERTEX_BIT);
	shader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/Gizmo.frag").string()), "", VK_SHADER_STAGE_FRAGMENT_BIT);

	// 创建管线
	// VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	// vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// auto bindingDescription = GetBindingDescription();
	// auto attributeDescriptions = GetAttributeDescriptions();
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

	// VkPipelineDepthStencilStateCreateInfo depthStencil{};
	// depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	// depthStencil.depthTestEnable = VK_FALSE;
	// depthStencil.depthWriteEnable = VK_FALSE;
	// depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	// depthStencil.depthBoundsTestEnable = VK_FALSE;
	// depthStencil.minDepthBounds = 0.0f; // Optional
	// depthStencil.maxDepthBounds = 1.0f; // Optional
	// depthStencil.stencilTestEnable = VK_FALSE;
	// depthStencil.front = {}; // Optional
	// depthStencil.back = {}; // Optional

	// vkPipeline = pipeline->CreatePipeline(vertShaderModule, fragShaderModule, false, vertexInputInfo, inputAssembly, depthStencil, vkPipelineLayout, renderPass->vkRenderPass, false);

	pipeline = std::make_shared<Pipeline>(m_device, shader, Application::Instance->renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL);
	CreatePushConstantPipelineLayout(pipeline->descriptorResource);

	uniformBuffer = std::make_shared<UniformBuffers>(m_device);
	uniformBuffer->UpdateWriteDescriptorSet(pipeline->descriptorResource);
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
void TransformGizmo::Draw(const std::shared_ptr<Camera>& camera, const VkCommandBuffer& currentCommandBuffer,
                          const std::shared_ptr<DescriptorResource>& descriptorResource, uint32_t currentFrame, GLFWwindow* window, const glm::mat4& inProjection)
{
	projection = inProjection;
	float distance = glm::length(referenceGameObject->transform->position - camera->position);
	float currentScaleFactor = distance * DISTANCE_SIZE_FACTOR; // 根据距离计算缩放因子
	auto scaleMatrix = glm::scale(modelMatrix, glm::vec3(currentScaleFactor * BASE_SIZE));
	if (abs(scaleFactor - currentScaleFactor) > 0.01f)
	{
		scaleFactor = currentScaleFactor;
		gizmoAABB[0] = ConvertToAABB(arrowX, scaleMatrix);
		gizmoAABB[1] = ConvertToAABB(arrowY, scaleMatrix);
		gizmoAABB[2] = ConvertToAABB(arrowZ, scaleMatrix);
	}
	vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);


	VkBuffer vertexBuffers[] = {buffer->vkBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &pipeline->descriptorResource->vkDescriptorSets[currentFrame], 0, nullptr);
	vkCmdPushConstants(currentCommandBuffer, vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &scaleMatrix);
	vkCmdDraw(currentCommandBuffer, static_cast<uint32_t>(gizmoData.size()), 1, 0, 0);
	auto ray = CursorPositionToWorldRay(window, camera->GetViewMatrix(), projection);
	float factor;
	glm::vec3 intersectPoint;
	auto leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	if (!leftMousePressed)
	{
		gizmoActiveX = gizmoActiveY = gizmoActiveZ = false;
	}
	else if (!gizmoActiveX && !gizmoActiveY && !gizmoActiveZ)
	{
		auto isActiveX = CollisionDetect::IntersectRayAABB(ray, gizmoAABB[0], factor, intersectPoint);
		auto isActiveY = CollisionDetect::IntersectRayAABB(ray, gizmoAABB[1], factor, intersectPoint);
		auto isActiveZ = CollisionDetect::IntersectRayAABB(ray, gizmoAABB[2], factor, intersectPoint);
		gizmoActiveX = isActiveX;
		gizmoActiveY = !gizmoActiveX && isActiveY;
		gizmoActiveZ = !gizmoActiveX && !gizmoActiveY && isActiveZ;
	}
	ApplyGizmoMovement(window, gizmoActiveX || gizmoActiveY || gizmoActiveZ);
}



Ray TransformGizmo::CursorPositionToWorldRay(GLFWwindow* window, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	float x = (2.0f * static_cast<float>(mouseX)) / static_cast<float>(windowWidth) - 1.0f;
	float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / static_cast<float>(windowHeight);
	float z = 1.0f; // 对于射线来说，Z分量通常设置为1
	glm::vec3 rayNds = glm::vec3(x, y, z);

	glm::vec4 rayClip = glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::vec4 rayWorld = glm::inverse(viewMatrix) * rayEye;
	auto direction = glm::normalize(glm::vec3(rayWorld));
	auto origin = glm::vec3(glm::inverse(viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	return Ray(origin, direction);
}

void TransformGizmo::UpdateGizmoAndObjectPosition(GLFWwindow* window, const std::shared_ptr<Camera>& camera)
{
	auto ray = CursorPositionToWorldRay(window, camera->GetViewMatrix(), projection); // 获取射线起点和方向
	glm::vec3 rayOrigin = ray.origin;
	glm::vec3 rayDir = ray.direction;

	// 假设Gizmo距离摄像机的深度为distance
	float distance = glm::length(referenceGameObject->transform->position - camera->position);

	// 使用射线与深度确定的平面交点来更新Gizmo位置
	// 假设平面方程为Ax + By + Cz + D = 0，这里选择与摄像机视线垂直的平面
	glm::vec3 planeNormal = glm::normalize(camera->front); // 摄像机前向向量作为平面法线
	float D = -glm::dot(planeNormal, camera->position + planeNormal * distance); // 计算平面方程的D值

	// 计算射线与平面的交点
	float denom = glm::dot(planeNormal, rayDir);
	if (abs(denom) > FLT_EPSILON)
	{
		// 确保不是平行（避免除以零）
		float t = -(glm::dot(planeNormal, rayOrigin) + D) / denom;
		glm::vec3 intersectionPoint = rayOrigin + rayDir * t;

		// 计算移动量
		glm::vec3 movement = intersectionPoint - referenceGameObject->transform->position;
		// 根据激活的Gizmo轴选择移动方向
		glm::vec3 axis = gizmoActiveX ? glm::vec3(1.0f, 0.0f, 0.0f) : gizmoActiveY ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);

		// 计算沿着Gizmo轴的移动量
		float movementAmount = glm::dot(movement, axis);
		auto movementAlongAxis = movementAmount * axis;
		// 更新Gizmo和关联对象的位置
		referenceGameObject->transform->position += movementAlongAxis;

		modelMatrix = glm::translate(modelMatrix, movementAlongAxis);
	}
}
