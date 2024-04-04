#pragma once
#include "PbrRendererSource.hpp"
#include "RendererSource.hpp"

namespace Sandbox
{
    class UnlitRendererSource : public PbrRendererSource
    {
    public:
        void CreatePipeline(std::shared_ptr<Renderer>& renderer) override;
    };
}  // namespace Sandbox
