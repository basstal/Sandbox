#pragma once
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>


namespace Sandbox
{
    class Device;
    struct SimpleVertex;
    class Buffer;

    class Ray
    {
    public:
        /**
         * \brief  起点
         */
        glm::vec3 origin;
        /**
         * \brief  方向
         */
        glm::vec3 direction;

        /**
         * \brief  构造函数
         * \param  origin 起点
         * \param  direction 方向
         */
        Ray(const glm::vec3& origin, const glm::vec3& direction);

        Ray();

        /**
         * \brief  获取射线上的点
         * \param  t 参数
         * \return  点
         */
        glm::vec3 PointAt(float t) const;


        /**
         * \brief  转换为字符串
         * \return  字符串
         */
        std::string ToString();

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
    };
}
