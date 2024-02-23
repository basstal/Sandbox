#pragma once
#include <glm/vec3.hpp>

#include "Math/AABB.hpp"
#include "Math/Plane.hpp"
#include "Math/Ray.hpp"

class CollisionDetect
{
public:
	static bool IntersectRayAABB(const Ray& ray, const AABB& aabb, float& factor, glm::vec3& intersectPoint);
	static float IntersectRayPlane(const Ray& ray, const Plane& plane);
};
