#pragma once
#include <memory>
#include <vector>

#include "SimpleVertex.hpp"
#include "GameCore/GameObject.hpp"
#include "Infrastructures/Math/Ray.hpp"
#include "Rendering/Buffers/Buffer.hpp"
#include "Rendering/Components/Pipeline.hpp"

class TransformGizmo
{
private:
	bool m_cleaned = false;
	std::shared_ptr<Device> m_device;

public:
	bool gizmoActiveX;
	bool gizmoActiveY;
	bool gizmoActiveZ;
	float radiusCylinder = 0.1f;
	float heightCylinder = 1.0f;
	float radiusCone = 0.2f;
	float heightCone = 0.5f;
	float scaleFactor;
	glm::mat4 modelMatrix;
	glm::mat4 projection;
	std::shared_ptr<GameObject> referenceGameObject;
	std::shared_ptr<Buffer> buffer;
	std::vector<SimpleVertex> gizmoData = std::vector<SimpleVertex>();
	std::array<AABB, 3> gizmoAABB = std::array<AABB, 3>();
	std::vector<SimpleVertex> arrowX;
	std::vector<SimpleVertex> arrowY;
	std::vector<SimpleVertex> arrowZ;
	void ApplyGizmoMovement(GLFWwindow* window, bool active);
	TransformGizmo(std::shared_ptr<GameObject> target, const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline,
	               std::shared_ptr<DescriptorResource> descriptorResource, const std::shared_ptr<RenderPass>& renderPass);
	~TransformGizmo();
	void Cleanup();
	std::vector<SimpleVertex> GenerateArrowData(glm::vec3 color);
	VkPipeline vkPipeline;
	VkPipelineLayout vkPipelineLayout;
	std::shared_ptr<Pipeline> pipeline;
	std::shared_ptr<UniformBuffers> uniformBuffer;
	void PrepareDrawData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource,
	                     const std::shared_ptr<DescriptorResource>& descriptorResource, const std::shared_ptr<RenderPass>& renderPass);
	void CreatePushConstantPipelineLayout(const std::shared_ptr<DescriptorResource>& descriptorResource);
	void Draw(const std::shared_ptr<Camera>& camera, const VkCommandBuffer& currentCommandBuffer,
	          const std::shared_ptr<DescriptorResource>& descriptorResource, uint32_t currentFrame, GLFWwindow* window, const glm::mat4& projection);
	Ray CursorPositionToWorldRay(GLFWwindow* window, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void UpdateGizmoAndObjectPosition(GLFWwindow* window, const std::shared_ptr<Camera>& camera);
};
