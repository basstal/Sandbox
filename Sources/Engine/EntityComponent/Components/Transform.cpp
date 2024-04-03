#include "pch.hpp"

#include "Generated/Transform.rfks.h"
#include "Transform.hpp"

void Sandbox::Transform::Cleanup() {}

glm::mat4 Sandbox::Transform::GetModelMatrix()
{
    // 合并矩阵: 注意乘法顺序是反向的
    return glm::translate(rotation.ToGlmMatrix4x4() * glm::scale(glm::mat4(1.0f), scale.ToGlmVec3()), position.ToGlmVec3());
}

glm::mat4 Sandbox::Transform::GetModelTranslate() { return glm::translate(glm::mat4(1.0f), position.ToGlmVec3()); }
