#pragma once
#include <vector>

#include "Engine/EntityComponent/IComponent.hpp"
#include "Engine/Model.hpp"
#include "Generated/Mesh.rfkh.h"
#include "Misc/String.hpp"

namespace Sandbox NAMESPACE()
{
    class Buffer;
    class Model;
    class CommandBuffer;
    class Device;
    class Material;

    class CLASS() Mesh : public IComponent
    {
    public:
        Mesh();

        void Cleanup() override;

        void LoadFromModel(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer, const Model& model);


        bool SubmitModelToDevice(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer);
        void RegisterModelToPhysicsWorld();

        std::shared_ptr<Model> loadedModel;

        std::shared_ptr<Buffer> vertexBuffer;
        std::shared_ptr<Buffer> indexBuffer;

        std::shared_ptr<Material> material;

        uint32_t Indices() const;

        std::string GetDerivedClassName() override;

        const rfk::Class* GetDerivedClass() override;

        bool isLoaded = false;

        std::string GetModelPath();

        void SetModelPath(const std::string& inPath);

        JPH::BodyID GetPhysicsBodyId() const;

    private:
        void LoadModelFromFile(const File& inFile);

        FIELD()
        String m_modelPath;

        JPH::BodyID m_physicsBodyId;

        std::vector<Vertex>   m_vertices;
        std::vector<uint32_t> m_indices;

        Sandbox_Mesh_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Mesh_GENERATED
