#pragma once
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Plane
{
public:
	glm::vec4 value;
	Plane(const glm::vec3& point, const glm::vec3& normal);
	glm::vec3 Normal() const;
};
