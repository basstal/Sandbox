#pragma once
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan_core.h>

struct Vertex
{
	glm::vec3 position;
	// glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texCoordinate;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
};
