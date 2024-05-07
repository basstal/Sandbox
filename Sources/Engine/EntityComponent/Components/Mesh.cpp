#include "pch.hpp"

#include "Mesh.hpp"

#include "Engine/EntityComponent/Scene.hpp"
#include "Engine/Model.hpp"
#include "Engine/PhysicsSystem.hpp"
#include "FileSystem/Directory.hpp"
#include "Generated/Mesh.rfks.h"
#include "Material.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/Device.hpp"

Sandbox::Mesh::Mesh()
{
    onComponentCreate.Bind(
        [this](const std::shared_ptr<IComponent>& reference)
        {
            if (reference.get() == this)
            {
                auto modelAssetPath = Directory::GetAssetsDirectory().GetFile(m_modelPath.ToStdString());
                LoadModelFromFile(modelAssetPath);
                material = gameObject.lock()->AddComponent<Material>();
            }
        });
}

void Sandbox::Mesh::Cleanup()
{
    indexBuffer != nullptr ? indexBuffer->Cleanup() : void();
    vertexBuffer != nullptr ? vertexBuffer->Cleanup() : void();
    isLoaded = false;
}

void Sandbox::Mesh::LoadFromModel(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer, const Model& model)
{
    // m_vertices                    = model.vertices;
    // m_indices                     = model.indices;
    // VkDeviceSize vertexBufferSize = sizeof(Vertex) * m_vertices.size();
    // vertexBuffer = std::make_shared<Buffer>(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // // TODO: 临时填充待绘制的数据
    // commandBuffer->CopyDataToBuffer(m_vertices.data(), vertexBufferSize, vertexBuffer);
    //
    // VkDeviceSize indexBufferSize = sizeof(uint32_t) * m_indices.size();
    // indexBuffer = std::make_shared<Buffer>(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // commandBuffer->CopyDataToBuffer(m_indices.data(), indexBufferSize, indexBuffer);
    // isLoaded = true;
}
void Sandbox::Mesh::LoadModelFromFile(const File& inFile)
{
    if (!inFile.Exists())
    {
        LOGW("Engine", "Model file not found. {}", inFile.path.string())
        return;
    }
    loadedModel                              = std::make_shared<Model>(inFile.path.string());
    m_vertices                               = loadedModel->vertices;
    m_indices                                = loadedModel->indices;
    Scene::currentScene->isRenderMeshesDirty = true;
}

bool Sandbox::Mesh::SubmitModelToDevice(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandBuffer>& commandBuffer)
{
    if (loadedModel == nullptr)
    {
        return false;
    }
    if (!isLoaded)
    {
        VkDeviceSize vertexBufferSize = sizeof(Vertex) * m_vertices.size();
        vertexBuffer =
            std::make_shared<Buffer>(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        // TODO: 临时填充待绘制的数据
        commandBuffer->CopyDataToBuffer(m_vertices.data(), vertexBufferSize, vertexBuffer);

        VkDeviceSize indexBufferSize = sizeof(uint32_t) * m_indices.size();
        indexBuffer =
            std::make_shared<Buffer>(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        commandBuffer->CopyDataToBuffer(m_indices.data(), indexBufferSize, indexBuffer);
        isLoaded = true;
    }
    return isLoaded;
}

void Sandbox::Mesh::RegisterModelToPhysicsWorld()
{
    if (!m_physicsBodyId.IsInvalid())
    {
        return;
    }
    m_physicsBodyId = PhysicsSystem::Instance->RegisterModelToPhysicsWorld(loadedModel);

    PhysicsSystem::Instance->physicsBodyIdToGameObject[m_physicsBodyId] = gameObject.lock();
}

bool Sandbox::Mesh::IsValid() { return isLoaded; }

uint32_t Sandbox::Mesh::Indices() const { return static_cast<uint32_t>(m_indices.size()); }

std::string Sandbox::Mesh::GetDerivedClassName() { return getArchetype().getName(); }

const rfk::Class* Sandbox::Mesh::GetDerivedClass() { return &getArchetype(); }

std::string Sandbox::Mesh::GetModelPath() { return m_modelPath.ToStdString(); }

void Sandbox::Mesh::SetModelPath(const std::string& inPath)
{
    // 如果 inPath 和原来 path 不相等
    if (m_modelPath.ToStdString() != inPath)
    {
        m_modelPath         = inPath;
        auto modelAssetPath = Directory::GetAssetsDirectory().GetFile(m_modelPath.ToStdString());
        Cleanup();
        LoadModelFromFile(modelAssetPath);
    }
}
JPH::BodyID Sandbox::Mesh::GetPhysicsBodyId() const
{
    return m_physicsBodyId;
}
