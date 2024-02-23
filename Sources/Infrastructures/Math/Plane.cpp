#include "Plane.hpp"
Plane::Plane(const glm::vec3& point, const glm::vec3& normal)
{
	auto normalized = glm::normalize(normal);
	value.w = -glm::dot(normalized, point); // 注意这里取了负值
	value.x = normalized.x;
	value.y = normalized.y;
	value.z = normalized.z;
}

glm::vec3 Plane::Normal() const
{
	return glm::vec3(value.x, value.y, value.z);
}
