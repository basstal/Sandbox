#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vulkan/vulkan.h>

#include "Rendering/Buffers/UniformBuffer.hpp"

struct MVPObject;
class Device;
class CommandResource;
class RenderPass;
class Pipeline;
class DescriptorResource;
class Buffer;
struct SimpleVertex;

class Grid
{
private:
    std::shared_ptr<Device> m_device;
    bool m_cleaned = false;

public:
    float unitSize = 1.0f;
    float gridSize = 10.0f;
    glm::vec2 offset = glm::vec2(0.0f, 0.0f);
    glm::vec2 scale = glm::vec2(1.0f, 1.0f);
    glm::vec2 center = glm::vec2(0.0f, 0.0f);
    glm::vec2 size = glm::vec2(100.0f, 100.0f);
    VkPipeline vkPipeline;
    std::shared_ptr<Buffer> buffer;
    std::vector<SimpleVertex> lineListProperties;

    std::vector<SimpleVertex> GetLineListProperties();

    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<UniformBuffer<MVPObject>> uniformBuffer;

    Grid(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<RenderPass>& renderPass);

    ~Grid();

    void Cleanup();

    void PrepareDrawData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<RenderPass>& renderPass);

    void Draw(const std::shared_ptr<Device>& device, const VkCommandBuffer& currentCommandBuffer,
              const std::shared_ptr<DescriptorResource>& descriptorResource, uint32_t currentFrame);
};
