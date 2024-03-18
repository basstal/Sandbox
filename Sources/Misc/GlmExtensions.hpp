#pragma once
#include <string>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace Sandbox
{
    std::string ToString(glm::vec3 inValue);

    std::string ToString(glm::mat4 inValue);

    std::string ToString(const float* inValue, uint32_t size);
}
