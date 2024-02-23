#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <memory>

class Model
{
public:
	Model();
	~Model();
	const std::vector<Vertex> Vertices() const { return m_vertices; }
	const std::vector<uint32_t> Indices() const { return m_indices; }

	static std::shared_ptr<Model> LoadModel(const char* path);

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	//std::vector<Texture> textures;
	std::string m_directory;
};
