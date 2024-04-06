#include "pch.hpp"

#include "Vector3.hpp"

#include "Generated/Vector3.rfks.h"

Sandbox::Vector3::Vector3() {}

Sandbox::Vector3::Vector3(float scalar)
{
    vec = glm::vec3(scalar);
    Sync();
}

Sandbox::Vector3::Vector3(const glm::vec3& inGlmVec3)
{
    vec = inGlmVec3;
    Sync();
}

Sandbox::Vector3::Vector3(glm::vec3&& inGlmVec3)
{
    vec = std::move(inGlmVec3);
    Sync();
}
Sandbox::Vector3& Sandbox::Vector3::operator=(const float other[3])
{
    vec = glm::vec3(other[0], other[1], other[2]);
    Sync();
    return *this;
}

Sandbox::Vector3 Sandbox::Vector3::operator-() const { return Vector3(-vec); }

void Sandbox::Vector3::operator+=(const glm::vec3& inGlmVec3)
{
    vec += inGlmVec3;
    Sync();
}

void Sandbox::Vector3::operator+=(const Vector3& inGlmVec3)
{
    vec += inGlmVec3.vec;
    Sync();
}

const Sandbox::Vector3 Sandbox::Vector3::operator*(const glm::vec3& inGlmVec3) const { return Vector3(vec * inGlmVec3); }

const Sandbox::Vector3 Sandbox::Vector3::operator*(float velocity) const { return Vector3(vec * velocity); }

std::string Sandbox::Vector3::ToString() const { return std::format("({0}, {1}, {2})", x, y, z); }

void Sandbox::Vector3::Sync()
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

glm::vec3 Sandbox::Vector3::ToGlmVec3()
{
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}
