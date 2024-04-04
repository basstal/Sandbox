#include "pch.hpp"

#include "Quaternion.hpp"

#include "Generated/Quaternion.rfks.h"

void Sandbox::Quaternion::Sync()
{
    x = m_quat.x;
    y = m_quat.y;
    z = m_quat.z;
    w = m_quat.w;
}

glm::quat Sandbox::Quaternion::ToGlmQuaternion()
{
    m_quat.x = x;
    m_quat.y = y;
    m_quat.z = z;
    m_quat.w = w;
    return m_quat;
}

glm::mat4 Sandbox::Quaternion::ToGlmMatrix4x4() { return glm::mat4_cast(m_quat); }
