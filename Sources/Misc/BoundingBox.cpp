#include "pch.hpp"

#include "BoundingBox.hpp"
#include "Editor/Grid.hpp"

#include "VulkanRHI/Core/Buffer.hpp"

Sandbox::BoundingBox::BoundingBox(const std::vector<SimpleVertex>& vertices)
{
    min = glm::vec3(FLT_MAX);
    max = glm::vec3(-FLT_MAX);

    for (const auto& vertex : vertices)
    {
        auto& position = vertex.position;
        min.x = std::min(min.x, position.x);
        min.y = std::min(min.y, position.y);
        min.z = std::min(min.z, position.z);
        max.x = std::max(max.x, position.x);
        max.y = std::max(max.y, position.y);
        max.z = std::max(max.z, position.z);
    }
}

Sandbox::BoundingBox::BoundingBox(glm::vec3 inMin, glm::vec3 inMax)
{
    min = inMin;
    max = inMax;
}

std::vector<Sandbox::SimpleVertex> Sandbox::BoundingBox::GetVertices() const
{
    std::vector<SimpleVertex> vertices;
    vertices.reserve(8);
    constexpr auto color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertices.push_back(SimpleVertex{glm::vec3(min.x, min.y, min.z), color});
    vertices.push_back({glm::vec3(max.x, min.y, min.z), color});
    vertices.push_back({glm::vec3(max.x, max.y, min.z), color});
    vertices.push_back({glm::vec3(min.x, max.y, min.z), color});
    vertices.push_back({glm::vec3(min.x, min.y, max.z), color});
    vertices.push_back({glm::vec3(max.x, min.y, max.z), color});
    vertices.push_back({glm::vec3(max.x, max.y, max.z), color});
    vertices.push_back({glm::vec3(min.x, max.y, max.z), color});
    return vertices;
}

std::vector<uint32_t> Sandbox::BoundingBox::GetIndices() const
{
    // 使用 VK_PRIMITIVE_TOPOLOGY_LINE_LIST，定义12条线段
    std::vector<uint32_t> indices = {
        0, 1, 1, 2, 2, 3, 3, 0, // 底面
        4, 5, 5, 6, 6, 7, 7, 4, // 顶面
        0, 4, 1, 5, 2, 6, 3, 7 // 侧面连接线
    };
    return indices;
}

void Sandbox::BoundingBox::PrepareDebugDrawData(const std::shared_ptr<Device>& device)
{
    VkDeviceSize vertexSize = sizeof(SimpleVertex) * GetVertices().size();
    vertexBuffer = std::make_shared<Buffer>(device, vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer->Update(GetVertices().data());

    VkDeviceSize indexSize = sizeof(uint32_t) * GetIndices().size();
    indexBuffer = std::make_shared<Buffer>(device, indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexBuffer->Update(GetIndices().data());
}

void Sandbox::BoundingBox::Cleanup()
{
    vertexBuffer != nullptr ? vertexBuffer->Cleanup() : void();
    indexBuffer != nullptr ? indexBuffer->Cleanup() : void();
}

Sandbox::BoundingBox Sandbox::BoundingBox::Multiply(const glm::mat4& mat)
{
    auto inMin = glm::vec3(mat * glm::vec4(min, 1.0f));
    auto inMax = glm::vec3(mat * glm::vec4(max, 1.0f));
    return BoundingBox(inMin, inMax);
}
