#pragma once
#include "glm/fwd.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/ext/quaternion_common.hpp"

namespace Sandbox
{
    glm::vec3 RotateVectorByQuaternion(const glm::vec3& v, const glm::quat& q)
    {
        // 将向量v转换为四元数，实部为0
        glm::quat vQuat(0.0f, v.x, v.y, v.z);

        // 计算旋转：qvq*
        glm::quat qConj = glm::conjugate(q); // 使用共轭代替逆
        glm::quat rotatedQuat = q * vQuat * qConj;

        // 返回旋转后的向量部分
        return glm::vec3(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
    }
}
