#pragma once
#include <glm\glm.hpp>
#include <string>
#include "Shader.h"
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader shader);

private:
	void SetupMesh();

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<Texture> mTextures;
	unsigned int mVAO, mVBO, mEBO;
};

struct Quad
{
	static std::vector<Vertex> vertices;
	static std::vector<unsigned int> indices;
};

struct Cube
{
	static std::vector<Vertex> vertices;
	static std::vector<unsigned int> indices;
};