#pragma once
#include "Misc/Hasher.hpp"
#include "Standard/Dictionary.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"


namespace Sandbox
{
    class Pipeline;

    class PipelineCaching
    {
    public:
        // // TODO:这里记录 shader module 到 pipeline 的映射关系，当 shader module 有变更时，重载对应的 pipeline，重载也会进一步更新这个 Dictionary
        // Dictionary<uint64_t, std::shared_ptr<Sandbox::Pipeline>> shaderModuleToPipeline;

        std::shared_ptr<Sandbox::Pipeline> GetOrCreatePipeline(const std::shared_ptr<PipelineState>& pipelineState);

        void Cleanup();

        void ReloadAll();

    private:
        Dictionary<PipelineState, std::shared_ptr<Sandbox::Pipeline>> m_pipelines;

        std::shared_ptr<Sandbox::Pipeline> CreatePipeline(const std::shared_ptr<PipelineState>& pipelineState);
    };


}  // namespace Sandbox