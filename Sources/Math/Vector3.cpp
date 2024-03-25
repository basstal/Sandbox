#include "pch.hpp"

#include "Generated/Vector3.rfks.h"

Sandbox::Vector3::Vector3()
{
}

Sandbox::Vector3::Vector3(float scalar)
{
    m_vec = glm::vec3(scalar);
    Sync();
}

Sandbox::Vector3::Vector3(const glm::vec3& inGlmVec3)
{
    m_vec = inGlmVec3;
    Sync();
}

Sandbox::Vector3 Sandbox::Vector3::operator-() const
{
    return Vector3(-m_vec);
}

void Sandbox::Vector3::operator+=(const glm::vec3& inGlmVec3)
{
    m_vec += inGlmVec3;
    Sync();
}

void Sandbox::Vector3::operator+=(const Vector3& inGlmVec3)
{
    m_vec += inGlmVec3.m_vec;
    Sync();
}

const Sandbox::Vector3 Sandbox::Vector3::operator*(const glm::vec3& inGlmVec3) const
{
    return Vector3(m_vec * inGlmVec3);
}

const Sandbox::Vector3 Sandbox::Vector3::operator*(float velocity) const
{
    return Vector3(m_vec * velocity);
}

void Sandbox::Vector3::Sync()
{
    x = m_vec.x;
    y = m_vec.y;
    z = m_vec.z;
}

glm::vec3 Sandbox::Vector3::ToGlmVec3()
{
    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    return m_vec;
}
