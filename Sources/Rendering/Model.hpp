#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <memory>

#include "GameCore/Transform.hpp"


class Model
{
public:
	Model();
	~Model();
	const std::vector<Vertex> vertices() const { return m_vertices; }
	const std::vector<uint32_t> indices() const { return m_indices; }

	static std::shared_ptr<Model> loadModel(const char* path);

private:
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	//std::vector<Texture> textures;
	std::string m_directory;
};
