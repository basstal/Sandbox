#pragma once
#include <glm/vec3.hpp>


class Light
{
public:
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 color;
	Light(glm::vec3 position, glm::vec3 color) :
		position(position), color(color)
	{
	}
	~Light() = default;
};
