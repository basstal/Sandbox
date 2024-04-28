#pragma once
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/Device.hpp"

namespace Sandbox
{
    /**
     * \brief 简单的顶点结构
     */
    struct SimpleVertexFlat
    {
        glm::vec2 position;
        glm::vec2 uv;
    };

    class BasicMeshData
    {
    public:
        // TODO:用 hash 来共享数据
        static std::shared_ptr<BasicMeshData> Instance;

        void Prepare(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer);

        void Cleanup();

        std::shared_ptr<Buffer> fullScreenQuad;
        uint32_t                fullScreenQuadPoints;

    private:
        bool m_cleaned = false;
    };
}  // namespace Sandbox
