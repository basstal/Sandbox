#include "pch.hpp"

#include "Material.hpp"

#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Generated/Material.rfks.h"
#include "Mesh.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"

Sandbox::Material::Material()
{
    onComponentCreate.Bind(
        [this](const std::shared_ptr<IComponent>& inComponent)
        {
            if (inComponent.get() == this)
            {
                m_mesh = gameObject.lock()->GetComponent<Mesh>();
            }
        });
}
void Sandbox::Material::DrawMesh(const std::shared_ptr<PipelineLayout>& pipelineLayout, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                                 const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets)
{
    // TODO:临时写的，这里应该还需要再设计一下
    if (customRendererSource == nullptr)
    {
        commandBuffer->BindDescriptorSet(pipelineLayout, rendererSource->descriptorSets[frameFlightIndex], {dynamicOffsets});
        commandBuffer->BindVertexBuffers(m_mesh->vertexBuffer);
        commandBuffer->BindIndexBuffer(m_mesh->indexBuffer);
        commandBuffer->DrawIndexed(m_mesh->Indices());
    }
    else
    {
        customRendererSource->CustomDrawMesh(m_mesh, commandBuffer, rendererSource->descriptorSets[frameFlightIndex], frameFlightIndex, dynamicOffsets);
    }
}

void Sandbox::Material::DrawOverlay(const std::shared_ptr<PipelineLayout>& pipelineLayout, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                                    const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets)
{
    if (customRendererSource == nullptr)
    {
        // pass
    }
    else
    {
        customRendererSource->CustomDrawOverlay(m_mesh, commandBuffer, rendererSource->descriptorSets[frameFlightIndex], frameFlightIndex, dynamicOffsets);
    }
}
