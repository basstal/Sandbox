#pragma once
#include <glm/vec3.hpp>

/**
 * \brief 光源
 */
struct Light
{
    /**
     * \brief 位置
     */
    alignas(16) glm::vec3 position;
    /**
     * \brief 颜色
     */
    alignas(16) glm::vec3 color;
};
