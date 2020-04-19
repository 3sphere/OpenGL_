#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
	mVertices(vertices),
	mIndices(indices),
	mTextures(textures)
{
	SetupMesh();
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), &mIndices[0], GL_STATIC_DRAW);

	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
	/* CONVENTION: */
	// assume that each sampler2D in the shader is called either texture_diffuseN or
	// texture_specularN, where N ranges from 1 to the maximum number of texture units allowed
	int diffuseNum = 1, specularNum = 1;
	for (int i = 0; i < mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = mTextures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNum++);
		else if (name == "texture_specular")
			number = std::to_string(specularNum++);

		shader.SetInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Draw
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<Vertex> Quad::vertices =
{
	{glm::vec3(-0.5f, -0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f)},
	{glm::vec3(0.5f, -0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  0.0f)},
	{glm::vec3(0.5f,  0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f)},
	{glm::vec3(-0.5f,  0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  1.0f)},
};

std::vector<unsigned int> Quad::indices =
{
	0, 1, 2,
	2, 3, 0
};

std::vector<Vertex> Cube::vertices =
{
	// front face
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 0.0f,  0.0f)},
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 1.0f,  0.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 0.0f,  1.0f)},
	// rear face															    
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 0.0f,  0.0f)},
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 1.0f,  0.0f)},
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 0.0f,  1.0f)},
	// left face															    
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 0.0f,  0.0f)},
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 1.0f,  0.0f)},
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 0.0f,  1.0f)},
	// right face															    
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 0.0f,  0.0f)},
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 0.0f,  1.0f)},
	// top face																    
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 0.0f,  0.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 1.0f,  0.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 0.0f,  1.0f)},
	// bottom face															    
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 0.0f,  0.0f)},
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 1.0f,  0.0f)},
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 1.0f,  1.0f)},
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 0.0f,  1.0f)}
};

std::vector<unsigned int> Cube::indices =
{
	// front face
	0, 1, 2,
	2, 3, 0,
	// rear face
	4, 5, 6,
	6, 7, 4,
	// left face
	8, 9, 10,
	10, 11, 8,
	// right face
	12, 13, 14,
	14, 15, 12,
	// top face
	16, 17, 18,
	18, 19, 16,
	// bottom face
	20, 21, 22,
	22, 23, 20
};