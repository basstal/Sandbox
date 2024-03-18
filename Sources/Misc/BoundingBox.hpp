#pragma once
#include <memory>
#include <vector>

#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

namespace Sandbox
{
    struct SimpleVertex;
    class Buffer;
    class Device;

    class BoundingBox
    {
    public:
        BoundingBox(const std::vector<SimpleVertex>& vertices);

        BoundingBox(glm::vec3 inMin, glm::vec3 inMax);

        /**
         * \brief  最小点
         */
        glm::vec3 min;
        /**
         * \brief  最大点
         */
        glm::vec3 max;
        std::shared_ptr<Buffer> vertexBuffer;
        std::shared_ptr<Buffer> indexBuffer;

        /**
         * \brief  获取顶点数据
         * \return  顶点数据
         */
        std::vector<SimpleVertex> GetVertices() const;

        /**
         * \brief  获取索引数据
         * \return  索引数据
         */
        std::vector<uint32_t> GetIndices() const;


        void PrepareDebugDrawData(const std::shared_ptr<Device>& device);

        void Cleanup();

        BoundingBox Multiply(const glm::mat4& mat);
    };
}
