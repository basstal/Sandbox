#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include <vector>
#include <string>

class Model
{
public:
	Model() {}
	Model(const char *path);
	~Model();
	const std::vector<Vertex> vertices() const { return _vertices; }
	const std::vector<uint32_t> indices() const { return _indices; }

private:
	void loadModel(const char *path);
	void processNode(aiNode *node, const aiScene *scene);
	void processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;
	//std::vector<Texture> textures;
	std::string _directory;
};
#endif // MODEL_H_INCLUDED