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

Sandbox::Quaternion& Sandbox::Quaternion::operator=(Quaternion&& other) noexcept
{
    m_quat = std::move(other.m_quat);
    Sync();
    return *this;
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

std::string Sandbox::Quaternion::ToString() const { return std::format("({0}, {1}, {2}, {3})", x, y, z, w); }

glm::vec3 Sandbox::Quaternion::GetEulerRadians() const { return glm::eulerAngles(m_quat); }

glm::vec3 Sandbox::Quaternion::GetEulerDegrees() const { return glm::degrees(GetEulerRadians()); }
