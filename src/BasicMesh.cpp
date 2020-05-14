#include "BasicMesh.h"

BasicMesh::BasicMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	: mTextures(textures)
{
	// Create tangents and bitangents
	for (int i = 0; i < indices.size(); i += 3)
	{
		glm::vec3 tangent, bitangent;
		Vertex vertex1 = vertices[indices[i]];
		Vertex vertex2 = vertices[indices[i + 1]];
		Vertex vertex3 = vertices[indices[i + 2]];

		glm::vec3 edge1 = vertex2.Position - vertex1.Position;
		glm::vec3 edge2 = vertex3.Position - vertex1.Position;
		glm::vec2 deltaUV1 = vertex2.TexCoords - vertex1.TexCoords;
		glm::vec2 deltaUV2 = vertex3.TexCoords - vertex1.TexCoords;

		float f = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.y = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		vertex1.Tangent = tangent;
		vertex1.Bitangent = bitangent;
		vertex2.Tangent = tangent;
		vertex2.Bitangent = bitangent;
		vertex3.Tangent = tangent;
		vertex3.Bitangent = bitangent;

		mVertices.push_back(vertex1);
		mVertices.push_back(vertex2);
		mVertices.push_back(vertex3);
	}

	SetupMesh();
}

void BasicMesh::Draw(Shader shader)
{
	/* CONVENTION: */
	// assume that each sampler2D in the shader is called texture_<type>N,
	// where N ranges from 1 to the maximum number of texture units allowed
	int diffuseNum = 1, specularNum = 1, normalNum = 1, displacementNum = 1;
	for (int i = 0; i < mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = mTextures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNum++);
		else if (name == "texture_specular")
			number = std::to_string(specularNum++);
		else if (name == "texture_normal")
			number = std::to_string(normalNum++);
		else if (name == "texture_displacement")
			number = std::to_string(displacementNum++);

		shader.SetInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}

	// Draw
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void BasicMesh::SetupMesh()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), &mVertices[0], GL_STATIC_DRAW);

	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);
	// Tangents
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(3);
	// Bitangents
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
}