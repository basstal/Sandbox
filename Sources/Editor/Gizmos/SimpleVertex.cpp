#include "SimpleVertex.hpp"

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>

#include "Infrastructures/Math/AABB.hpp"
#include "GameCore/Vertex.hpp"
// VkVertexInputBindingDescription GetBindingDescription()
// {
// 	VkVertexInputBindingDescription bindingDescription;
// 	bindingDescription.binding = 0;
// 	bindingDescription.stride = sizeof(SimpleVertex);
// 	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
// 	return bindingDescription;
// }
//
// std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
// {
// 	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
//
// 	attributeDescriptions[0].binding = 0;
// 	attributeDescriptions[0].location = 0;
// 	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
// 	attributeDescriptions[0].offset = offsetof(SimpleVertex, position);
//
// 	attributeDescriptions[1].binding = 0;
// 	attributeDescriptions[1].location = 1;
// 	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
// 	attributeDescriptions[1].offset = offsetof(SimpleVertex, color);
//
// 	return attributeDescriptions;
// }


AABB ConvertToAABB(const std::vector<SimpleVertex>& vertices, const glm::mat4& modelMatrix)
{
	AABB aabb;
	aabb.min = glm::vec3(FLT_MAX);
	aabb.max = glm::vec3(-FLT_MAX);

	for (const auto& vertex : vertices)
	{
		auto position = modelMatrix * glm::vec4(vertex.position, 1.0f);
		aabb.min.x = std::min(aabb.min.x, position.x);
		aabb.min.y = std::min(aabb.min.y, position.y);
		aabb.min.z = std::min(aabb.min.z, position.z);
		aabb.max.x = std::max(aabb.max.x, position.x);
		aabb.max.y = std::max(aabb.max.y, position.y);
		aabb.max.z = std::max(aabb.max.z, position.z);
	}

	return aabb;
}

