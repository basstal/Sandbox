#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <memory>

#include "IComponent.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/CommandResource.hpp"

class Buffer;

class Model : public IComponent
{
    bool m_cleaned = false;
    std::string m_path;

    void ProcessNode(aiNode* node, const aiScene* scene);

    void ProcessMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    //std::vector<Texture> textures;
    std::string m_directory;

public:
    Model();

    ~Model() override;

    void Cleanup() override;

    const std::vector<Vertex> Vertices() const { return m_vertices; }

    const std::vector<uint32_t> Indices() const { return m_indices; }

    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;

    void PrepareRenderData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource);

    static std::shared_ptr<Model> LoadModel(const char* path);

    void Update() override;
};
