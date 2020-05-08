#pragma once
#include "Mesh.h"

class BasicMesh
{
public:
	BasicMesh() = default;
	BasicMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});
	void Draw(Shader shader);

private:
	void SetupMesh();

	std::vector<Vertex> mVertices;
	std::vector<Texture> mTextures;
	unsigned int mVAO, mVBO;
};