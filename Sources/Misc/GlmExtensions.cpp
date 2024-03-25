#include "pch.hpp"

#include "GlmExtensions.hpp"

#include <sstream>

std::string Sandbox::ToString(glm::vec3 inValue)
{
    std::stringstream ss;
    ss << "vec3(" << inValue.x << ", " << inValue.y << ", " << inValue.z << ")";
    return ss.str();
}

std::string Sandbox::ToString(glm::mat4 inValue)
{
    std::stringstream ss;
    ss << "mat4(\n";
    for (int i = 0; i < 4; i++)
    {
        ss << " " << inValue[0][i] << ", " << inValue[1][i] << ", " << inValue[2][i] << ", " << inValue[3][i] << "\n";
    }
    ss << ")";
    return ss.str();
}


std::string Sandbox::ToString(const float* inValue, uint32_t size)
{
    std::stringstream ss;
    for (size_t i = 0; i < size; ++i)
    {
        ss << inValue[i] << (i == size - 1 ? "" : ", ");
    }
    ss << "\n";
    return ss.str();
}
