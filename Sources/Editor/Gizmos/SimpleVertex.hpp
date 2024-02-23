#pragma once
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Infrastructures/Math/AABB.hpp"
#include "Rendering/Vertex.hpp"

struct SimpleVertex
{
	glm::vec3 position;
	glm::vec3 color;
};

VkVertexInputBindingDescription GetBindingDescription();
std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
AABB ConvertToAABB(const std::vector<SimpleVertex>& vertices, const glm::mat4& modelMatrix);
