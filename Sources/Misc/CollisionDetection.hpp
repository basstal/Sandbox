#pragma once
#include <glm/vec3.hpp>

namespace Sandbox
{
    class BoundingBox;
    class Ray;
    /**
     * \brief  射线与 包围盒 碰撞检测
     * \param  ray 射线
     * \param  boundingBox 包围盒
     * \param  factor 碰撞因子
     * \param  intersectPoint 碰撞点
     * \return  是否相交
     */
    bool IntersectRayBoundingBox(const Ray& ray, const BoundingBox& boundingBox, float& factor, glm::vec3& intersectPoint);
}
