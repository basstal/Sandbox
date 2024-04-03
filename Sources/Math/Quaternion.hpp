#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> // 包含GLM的四元数转换功能

#include "Generated/Quaternion.rfkh.h"
#include "Serialization/ISerializable.hpp"

namespace Sandbox NAMESPACE()
{
    class CLASS() Quaternion : public ISerializable<Quaternion>
    {
    public:
        FIELD()
        float x = 0.0f;
        FIELD()
        float y = 0.0f;
        FIELD()
        float z = 0.0f;
        FIELD()
        float w = 1.0f;

        // 默认构造函数
        Quaternion() = default;

        // 从glm::quat构造
        Quaternion(const glm::quat &inQuat) : m_quat(inQuat)
        {
            Sync();
        }

        // 复制构造函数
        Quaternion(const Quaternion &other) = default;

        // 移动构造函数
        Quaternion(Quaternion &&other) noexcept = default;

        // 从四个浮点数构造（w, x, y, z）
        Quaternion(float w, float x, float y, float z) : m_quat(w, x, y, z)
        {
            Sync();
        }

        // 析构函数
        virtual ~Quaternion() = default;

        // 赋值运算符
        Quaternion &operator=(const Quaternion &other) = default;

        // 移动赋值运算符
        Quaternion &operator=(Quaternion &&other) noexcept = default;

        // 四元数乘法
        Quaternion operator*(const Quaternion &rhs) const
        {
            return Quaternion(m_quat * rhs.m_quat);
        }

        // 转换为GLM的quat
        glm::quat ToGlmQuaternion();

        // 其他四元数操作...

        glm::mat4 ToGlmMatrix4x4();
    private:
        // 成员变量，使用glm::quat存储四元数
        glm::quat m_quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        void Sync();
        Sandbox_Quaternion_GENERATED
    };
} // namespace Sandbox

File_Quaternion_GENERATED
