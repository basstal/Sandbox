#pragma once

#include <glm/vec3.hpp>

#include "Editor.hpp"

namespace Sandbox
{
    class Device;
    class CommandBuffer;
    class Buffer;
    /**
     * \brief 简单的顶点结构
     */
    struct SimpleVertex
    {
        /**
         * \brief 顶点坐标
         */
        glm::vec3 position;
        /**
         * \brief 顶点颜色
         */
        glm::vec3 color;
    };

    class Grid
    {
    public:
        void Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor);

        void DrawGrid(const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t frameFlightIndex);

        void Cleanup();

        std::shared_ptr<Buffer>            lineListBuffer;
        std::vector<Sandbox::SimpleVertex> lineListProperties;
        std::shared_ptr<Device>            device;

    private:
        std::shared_ptr<Editor> m_editor;
        bool m_prepared = false;
    };
}
