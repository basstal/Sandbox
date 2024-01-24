#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <memory>

class Model
{
public:
	Model();
	~Model();
	const std::vector<Vertex> vertices() const { return _vertices; }
	const std::vector<uint32_t> indices() const { return _indices; }

	static std::shared_ptr<Model> loadModel(const char *path);
private:
	void processNode(aiNode *node, const aiScene *scene);
	void processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;
	//std::vector<Texture> textures;
	std::string _directory;
};