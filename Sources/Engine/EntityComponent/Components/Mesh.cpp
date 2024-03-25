#include "pch.hpp"

#include "Mesh.hpp"

#include "Engine/Model.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"

void Sandbox::Mesh::Cleanup()
{
    indexBuffer != nullptr ? indexBuffer->Cleanup() : void();
    vertexBuffer != nullptr ? vertexBuffer->Cleanup() : void();
}


void Sandbox::Mesh::LoadFromModel(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer, const Model& model)
{
    m_vertices = model.vertices;
    m_indices = model.indices;
    VkDeviceSize vertexBufferSize = sizeof(Vertex) * m_vertices.size();
    vertexBuffer = std::make_shared<Buffer>(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // TODO: 临时填充待绘制的数据
    commandBuffer->CopyDataToBuffer(m_vertices.data(), vertexBufferSize, vertexBuffer);

    VkDeviceSize indexBufferSize = sizeof(uint32_t) * m_indices.size();
    indexBuffer = std::make_shared<Buffer>(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    commandBuffer->CopyDataToBuffer(m_indices.data(), indexBufferSize, indexBuffer);
}

uint32_t Sandbox::Mesh::Indices() const
{
    return static_cast<uint32_t>(m_indices.size());
}
