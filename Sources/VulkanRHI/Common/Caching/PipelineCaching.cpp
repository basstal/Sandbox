#include "pch.hpp"

#include "PipelineCaching.hpp"

#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "ResourceCaching.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"

std::shared_ptr<Sandbox::Pipeline> Sandbox::PipelineCaching::GetOrCreatePipeline(const std::shared_ptr<PipelineState>& pipelineState)
{
    // uint64_t hash;
    // HashParam(hash, pipelineState);

    if (m_pipelines.contains(*pipelineState))
    {
        // LOGD("VulkanRHI", "reuse pipeline vkPipeline : {}", PtrToHexString(m_pipelines.at(*pipelineState)->vkPipeline))
        return m_pipelines.at(*pipelineState);
    }
    auto newPipeline            = CreatePipeline(pipelineState);
    m_pipelines[*pipelineState] = newPipeline;
    return newPipeline;
}
void Sandbox::PipelineCaching::Cleanup()
{
    for (auto& pipeline : m_pipelines)
    {
        pipeline.second->Cleanup();
    }
    m_pipelines.clear();
}

void Sandbox::PipelineCaching::ReloadAll()
{
    Dictionary<PipelineState, std::shared_ptr<Sandbox::Pipeline>> newPipelines;
    for (auto& [_, pipeline] : m_pipelines)
    {
        pipeline->Reload();
        newPipelines[*pipeline->m_pipelineState] = pipeline;
    }
    m_pipelines.clear();
    m_pipelines = newPipelines;
}

std::shared_ptr<Sandbox::Pipeline> Sandbox::PipelineCaching::CreatePipeline(const std::shared_ptr<PipelineState>& pipelineState)
{
    return std::make_shared<Pipeline>(pipelineState->renderPass->GetDevice(), pipelineState);
}
