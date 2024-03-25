#include "pch.hpp"

#include "Ray.hpp"
#include "GlmExtensions.hpp"
#include "Editor/Grid.hpp"
#include "VulkanRHI/Core/Buffer.hpp"

Sandbox::Ray::Ray(const glm::vec3& origin, const glm::vec3& direction):
    origin(origin), direction(direction)
{
}

Sandbox::Ray::Ray():
    origin(0.0f),
    direction(1.0f, 0.0f, 0.0f)
{
}

glm::vec3 Sandbox::Ray::PointAt(float t) const
{
    return origin + t * direction;
}

std::string Sandbox::Ray::ToString()
{
    return "Ray: origin: " + Sandbox::ToString(origin) + " direction: " + Sandbox::ToString(direction);
}

std::vector<Sandbox::SimpleVertex> Sandbox::Ray::GetVertices() const
{
    return std::vector<SimpleVertex>
    {
        SimpleVertex{origin, glm::vec3(0.0f)},
        {PointAt(50), glm::vec3(1.0f)}
    };
}

std::vector<uint32_t> Sandbox::Ray::GetIndices() const
{
    std::vector<uint32_t> indices = {
        0, 1
    };
    return indices;
}

void Sandbox::Ray::PrepareDebugDrawData(const std::shared_ptr<Device>& device)
{
    if (!vertexBuffer)
    {
        VkDeviceSize vertexSize = sizeof(SimpleVertex) * GetVertices().size();
        vertexBuffer = std::make_shared<Buffer>(device, vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    vertexBuffer->Update(GetVertices().data());

    if (!indexBuffer)
    {
        VkDeviceSize indexSize = sizeof(uint32_t) * GetIndices().size();
        indexBuffer = std::make_shared<Buffer>(device, indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    indexBuffer->Update(GetIndices().data());
}

void Sandbox::Ray::Cleanup()
{
    vertexBuffer != nullptr ? vertexBuffer->Cleanup() : void();
    indexBuffer != nullptr ? indexBuffer->Cleanup() : void();
}
