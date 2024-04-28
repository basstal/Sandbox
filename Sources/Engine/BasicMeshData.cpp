#include "pch.hpp"

#include "BasicMeshData.hpp"

#include "Misc/TypeCasting.hpp"
#include "RendererSource/BufferRendererSource.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"

std::vector<Sandbox::SimpleVertexFlat> GetQuadProperties()
{
    // 构造 vulkan 全屏四边形
    std::vector<Sandbox::SimpleVertexFlat> quadBuffer = {
        {{-1.0f, 1.0f}, {0.0f, 1.0f}}, {{1.0f, -1.0f}, {1.0f, 0.0f}}, {{-1.0f, -1.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f}, {0.0f, 1.0f}}, {{1.0f, 1.0f}, {1.0f, 1.0f}},  {{1.0f, -1.0f}, {1.0f, 0.0f}},
    };
    return quadBuffer;
}

std::shared_ptr<Sandbox::BasicMeshData> Sandbox::BasicMeshData::Instance = nullptr;

void Sandbox::BasicMeshData::Prepare(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer)
{
    auto quadProperties = GetQuadProperties();
    auto bufferSize     = sizeof(SimpleVertexFlat) * quadProperties.size();
    fullScreenQuad = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    fullScreenQuadPoints = ToUInt32(quadProperties.size());
    commandBuffer->CopyDataToBuffer(quadProperties.data(), bufferSize, fullScreenQuad);
    m_cleaned = false;
}

void Sandbox::BasicMeshData::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    fullScreenQuad != nullptr ? fullScreenQuad->Cleanup() : void();
}
