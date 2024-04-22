#include "pch.hpp"

#include "Grid.hpp"

#include "Editor.hpp"
#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Misc/TypeCasting.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"

constexpr float UNIT_SIZE = 1.0f;
constexpr float GRID_SIZE = 10.0f;
// constexpr glm::vec2 OFFSET = glm::vec2(0.0f, 0.0f);
// constexpr glm::vec2 SCALE = glm::vec2(1.0f, 1.0f);
// constexpr glm::vec2 CENTER = glm::vec2(0.0f, 0.0f);
constexpr glm::vec2 TOTAL_AREA  = glm::vec2(100.0f, 100.0f);
constexpr auto      GRAY        = glm::vec3(0.5f, 0.5f, 0.5f);
constexpr auto      DEEPER_GRAY = glm::vec3(0.3f, 0.3f, 0.3f);
constexpr auto      RED         = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr auto      GREEN       = glm::vec3(0.0f, 1.0f, 0.0f);

std::vector<Sandbox::SimpleVertex> GetLineListProperties()
{
    std::vector<Sandbox::SimpleVertex> lineListProperty;
    auto                               halfSize = glm::vec2(TOTAL_AREA.x / 2, TOTAL_AREA.y / 2);
    // 使中央的线段 x 方向用红色，y 方向用绿色
    float i = -halfSize.x;
    for (; i <= halfSize.x;)
    {
        Sandbox::SimpleVertex property;
        property.position = glm::vec3(i, -halfSize.y, 0.0f);
        if (i == 0.0f)
        {
            property.color = GREEN;
        }
        else if (static_cast<int>(i) % static_cast<int>(GRID_SIZE) == 0)
        {
            property.color = DEEPER_GRAY;
        }
        else
        {
            property.color = GRAY;
        }
        lineListProperty.push_back(property);
        property.position = glm::vec3(i, halfSize.y, 0.0f);
        lineListProperty.push_back(property);
        i += UNIT_SIZE;
    }
    i = -halfSize.y;
    for (; i <= halfSize.y;)
    {
        Sandbox::SimpleVertex property;
        property.position = glm::vec3(-halfSize.x, i, 0.0f);
        if (i == 0.0f)
        {
            property.color = RED;
        }
        else if (static_cast<int>(i) % static_cast<int>(GRID_SIZE) == 0)
        {
            property.color = DEEPER_GRAY;
        }
        else
        {
            property.color = GRAY;
        }
        lineListProperty.push_back(property);
        property.position = glm::vec3(halfSize.x, i, 0.0f);
        lineListProperty.push_back(property);
        i += UNIT_SIZE;
    }
    return lineListProperty;
}

void Sandbox::Grid::Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor)
{
    m_editor           = editor;
    lineListProperties = GetLineListProperties();
    device             = renderer->device;
    // auto pipelineLayout = renderer->pipelineLayout;
    auto         commandBuffer = renderer->commandBuffers[0];
    VkDeviceSize bufferSize    = sizeof(SimpleVertex) * lineListProperties.size();
    lineListBuffer = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    commandBuffer->CopyDataToBuffer(lineListProperties.data(), bufferSize, lineListBuffer);
    m_prepared = true;
}

void Sandbox::Grid::DrawGrid(const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t frameFlightIndex)
{
    uint32_t dynamicAlignment = m_editor->GetUniformDynamicAlignment(sizeof(glm::mat4));
    commandBuffer->BindPipeline(m_editor->pipelineLineList);
    auto offset = Scene::currentScene != nullptr ? Scene::currentScene->renderMeshes.size() : 0;
    commandBuffer->BindDescriptorSet(m_editor->pipelineLineList->pipelineLayout, m_editor->descriptorSets[frameFlightIndex],
                                     {static_cast<uint32_t>(offset * dynamicAlignment)});
    commandBuffer->BindVertexBuffers(lineListBuffer);
    commandBuffer->Draw(ToUInt32(lineListProperties.size()));
}

void Sandbox::Grid::Cleanup()
{
    if (!m_prepared)
    {
        return;
    }
    m_prepared = false;
    lineListBuffer->Cleanup();
}
