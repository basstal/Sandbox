#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Rendering/Vertex.hpp"

struct SimpleVertex
{
	glm::vec3 position;
	glm::vec3 color;
};

VkVertexInputBindingDescription getBindingDescription();
std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
