#include "pch.hpp"

#include "PipelineCaching.hpp"

#include "ResourceCaching.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"

std::shared_ptr<Sandbox::Pipeline> Sandbox::PipelineCaching::GetOrCreatePipeline(const std::shared_ptr<PipelineState>& pipelineState)
{
    uint64_t hash;
    HashParam(hash, pipelineState);
    if (pipelines.contains(hash))
    {
        return pipelines.at(hash);
    }
    auto newPipeline = CreatePipeline(pipelineState);
    pipelines[hash]  = newPipeline;
    return newPipeline;
}
void Sandbox::PipelineCaching::Cleanup()
{
    for (auto& pipeline : pipelines)
    {
        pipeline.second->Cleanup();
    }
    pipelines.clear();
}

std::shared_ptr<Sandbox::Pipeline> Sandbox::PipelineCaching::CreatePipeline(const std::shared_ptr<PipelineState>& pipelineState)
{
    return std::make_shared<Pipeline>(pipelineState->renderPass->GetDevice(), pipelineState);
}
