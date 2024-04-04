#include "pch.hpp"

#include "UnlitRendererSource.hpp"

#include "FileSystem/Directory.hpp"
#include "VulkanRHI/Renderer.hpp"

void Sandbox::UnlitRendererSource::CreatePipeline(std::shared_ptr<Renderer>& renderer) { CreatePipelineWithPreamble(renderer, "#define DISABLE_LIGHTS\n"); }
