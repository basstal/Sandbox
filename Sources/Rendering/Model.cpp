#include "Model.hpp"
#include <stdexcept>

#include "Vertex.hpp"

Model::Model()
{
}

Model::~Model()
{
}

std::shared_ptr<Model> Model::loadModel(const char *path)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
	}
	model->_directory = std::string(path).substr(0, std::string(path).find_last_of('/'));
	model->processNode(scene->mRootNode, scene);
	return model;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.color = { 1.0f, 1.0f, 1.0f };
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0f, 0.0f);
		}
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		_vertices.push_back(vertex);
	}
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			_indices.push_back(face.mIndices[j]);
		}
	}
}
