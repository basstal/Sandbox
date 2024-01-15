#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif