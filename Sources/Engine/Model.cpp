#include "Model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "FileSystem/Logger.hpp"

Sandbox::Model::Model(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Logger::Fatal("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
    }
    // m_directory = std::string(path).substr(0, std::string(path).find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}

void Sandbox::Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Sandbox::Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // vertex.color = {1.0f, 1.0f, 1.0f};
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoordinate = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.texCoordinate = glm::vec2(0.0f, 0.0f);
        }
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertices.push_back(vertex);
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
}
