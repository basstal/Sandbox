#pragma once
#include <glm/vec3.hpp>

class Ray
{
public:
	glm::vec3 origin;
	glm::vec3 direction;
	Ray(const glm::vec3& origin, const glm::vec3& direction) :
		origin(origin), direction(direction)
	{
	}
	glm::vec3 PointAt(float t) const { return origin + t * direction; }
};
