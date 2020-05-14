#pragma once
#include <string>
#include "Shader.h"
#include <vector>
#include "Mesh.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

class Model
{
public:
	Model() = default;
	Model(const std::string& path);
	void Draw(Shader shader);

private:
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	std::vector<Mesh> mMeshes;
	std::string mDirectory;
	std::vector<Texture> mLoadedTextures;
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gammaCorrection);