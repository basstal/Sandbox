#include "CollisionDetect.hpp"

#include <glm/common.hpp>

#include "Math/AABB.hpp"
#include "Math/Plane.hpp"

bool CollisionDetect::IntersectRayAABB(const Ray& ray, const AABB& aabb, float& factor, glm::vec3& intersectPoint)
{
	factor = 0.0f;
	float tmax = FLT_MAX;
	auto d = ray.direction;
	auto p = ray.origin;
	for (int i = 0; i < 3; i++)
	{
		if (glm::abs(d[i]) < FLT_EPSILON)
		{
			if (p[i] < aabb.min[i] || p[i] > aabb.max[i])
			{
				return false;
			}
		}
		else
		{
			float ood = 1.0f / d[i];
			float t1 = (aabb.min[i] - p[i]) * ood;
			float t2 = (aabb.max[i] - p[i]) * ood;
			if (t1 > t2)
			{
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}
			factor = glm::max(factor, t1);
			tmax = glm::min(tmax, t2);
			if (factor > tmax)
			{
				return false;
			}
		}
	}
	intersectPoint = p + d * factor;
	return true;
}

float CollisionDetect::IntersectRayPlane(const Ray& ray, const Plane& plane)
{
	glm::vec3 planeNormal = plane.Normal();
	const float numer = glm::dot(planeNormal, ray.origin) - plane.value.w;
	const float denom = glm::dot(planeNormal, ray.direction);

	if (fabsf(denom) < FLT_EPSILON) // normal is orthogonal to vector, cant intersect
	{
		return -1.0f;
	}

	return -(numer / denom);
}
