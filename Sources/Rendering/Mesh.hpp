#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vulkan_core.h"

#include <glm/glm.hpp>
#include <vector>

#include "Vertex.hpp"


struct Texture
{
	unsigned int ID;
	// std::string Type;
	// aiString Path;
};

class Mesh
{
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	// void Draw(const Shader& shader);
	// void BindTexture(const Shader& shader);
	// unsigned int GetVAO();
private:
	// render data
	// unsigned int VAO, VBO, EBO;
	// void setupMesh();
};