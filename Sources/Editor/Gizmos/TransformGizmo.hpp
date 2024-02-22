#pragma once
#include <memory>
#include <vector>

#include "SimpleVertex.hpp"
#include "GameCore/GameObject.hpp"
#include "Rendering/Components/Buffer.hpp"
#include "Rendering/Components/Pipeline.hpp"

class TransformGizmo
{
private:
	bool m_cleaned = false;
	std::shared_ptr<Device> m_device;

public:
	float radiusCylinder = 0.1f;
	float heightCylinder = 1.0f;
	float radiusCone = 0.2f;
	float heightCone = 0.5f;
	glm::mat4 modelMatrix;
	std::shared_ptr<GameObject> referenceGameObject;
	std::shared_ptr<Buffer> buffer;
	std::vector<SimpleVertex> gizmoData = std::vector<SimpleVertex>();
	TransformGizmo(std::shared_ptr<GameObject> target, const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline,
	               std::shared_ptr<DescriptorResource> descriptorResource);
	~TransformGizmo();
	void Cleanup();
	std::vector<SimpleVertex> GenerateArrowData(glm::vec3 color);
	VkPipeline vkPipeline;
	VkPipelineLayout vkPipelineLayout;
	void PrepareDrawData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<DescriptorResource>&
	                     descriptorResource);
	void CreatePushConstantPipelineLayout(const std::shared_ptr<DescriptorResource>& descriptorResource);
	void Draw(const std::shared_ptr<Device>& device, const VkCommandBuffer& currentCommandBuffer, const std::shared_ptr<Pipeline>& pipeline,
	          const std::shared_ptr<DescriptorResource>& descriptorResource, uint32_t currentFrame);
	void AdjustGizmoSize(const std::shared_ptr<Camera>& camera);
};
