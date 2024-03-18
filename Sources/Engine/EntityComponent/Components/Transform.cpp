#include "Transform.hpp"

#include "glm/gtc/quaternion.hpp"

void Sandbox::Transform::Cleanup()
{
}

glm::mat4 Sandbox::Transform::GetModelMatrix() const
{
    // 合并矩阵: 注意乘法顺序是反向的
    return glm::translate(glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale), position);
}

glm::mat4 Sandbox::Transform::GetModelTranslate() const
{
    return glm::translate(glm::mat4(1.0f), position);
}
