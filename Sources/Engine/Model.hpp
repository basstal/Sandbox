#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct aiScene;
struct aiMesh;
struct aiNode;

namespace Sandbox
{
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

    class Model
    {
    public:
        Model(const std::string& path);

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

    private:
        void ProcessNode(aiNode* node, const aiScene* scene);

        void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
}
