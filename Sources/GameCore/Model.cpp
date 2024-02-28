#include "Model.hpp"
#include <stdexcept>

#include <assimp/postprocess.h>
#include "Vertex.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Buffers/IndexBuffer.hpp"
#include "Rendering/Buffers/VertexBuffer.hpp"

Model::Model()
{
}

Model::~Model()
{
}

void Model::PrepareRenderData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource)
{
	VkDeviceSize bufferSize = sizeof(Vertices()[0]) * Vertices().size();

	vertexBuffer = std::make_shared<VertexBuffer>(device, Vertices().data(), bufferSize, commandResource);
	VkDeviceSize indexBufferSize = sizeof(Indices()[0]) * Indices().size();

	indexBuffer = std::make_shared<IndexBuffer>(device, Indices().data(), indexBufferSize, commandResource);
}
std::shared_ptr<Model> Model::LoadModel(const char* path)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Fatal("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
	}
	model->m_directory = std::string(path).substr(0, std::string(path).find_last_of('/'));
	model->ProcessNode(scene->mRootNode, scene);
	return model;
}
void Model::Update()
{
	if (vertexBuffer == nullptr)
	{
		auto application = SingletonOrganizer::Get<Renderer>();
		PrepareRenderData(application->device, application->commandResource);
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
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

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
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
		m_vertices.push_back(vertex);
	}
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			m_indices.push_back(face.mIndices[j]);
		}
	}
}
