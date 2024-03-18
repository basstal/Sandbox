#include "CollisionDetection.hpp"

#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "glm/common.hpp"

bool Sandbox::IntersectRayBoundingBox(const Ray& ray, const BoundingBox& boundingBox, float& factor, glm::vec3& intersectPoint)
{
    factor = 0.0f;
    float tmax = FLT_MAX;
    auto d = ray.direction;
    auto p = ray.origin;
    for (int i = 0; i < 3; i++)
    {
        if (glm::abs(d[i]) < FLT_EPSILON)
        {
            if (p[i] < boundingBox.min[i] || p[i] > boundingBox.max[i])
            {
                return false;
            }
        }
        else
        {
            float ood = 1.0f / d[i];
            float t1 = (boundingBox.min[i] - p[i]) * ood;
            float t2 = (boundingBox.max[i] - p[i]) * ood;
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
