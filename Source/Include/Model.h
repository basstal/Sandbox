#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

class Model
{
public:
	Model(const char *path);
	~Model();
private:
	void loadModel(const char *path);
	void processNode(aiNode *node, const aiScene *scene);
	void processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	//std::vector<Texture> textures;
	std::string directory;
};
#endif // MODEL_H_INCLUDED