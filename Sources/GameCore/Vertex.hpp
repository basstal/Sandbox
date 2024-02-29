#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/**
 * @brief Vertex data structure
 */
struct Vertex
{
    /**
     * @brief Vertex position
     */
    glm::vec3 position;
    /**
     * @brief Vertex normal
     */
    glm::vec3 normal;
    /**
     * @brief Vertex texture coordinate
     */
    glm::vec2 texCoordinate;
};
