#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Engine/EntityComponent/IComponent.hpp"


namespace Sandbox
{
    class Buffer;
    class Model;
    class CommandBuffer;
    class Device;
    struct Vertex;

    class Mesh : public IComponent, public ISerializable<Mesh>
    {
    public:
        void Cleanup() override;

        void LoadFromModel(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer, const Model& model);


        std::shared_ptr<Buffer> vertexBuffer;
        std::shared_ptr<Buffer> indexBuffer;

        uint32_t Indices() const;

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };
}
