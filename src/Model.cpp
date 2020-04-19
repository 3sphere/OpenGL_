#include "Model.h"
#include <iostream>

Model::Model(const std::string& path)
{
	LoadModel(path);
}

void Model::Draw(Shader shader)
{
	for (int i = 0; i < mMeshes.size(); i++)
		mMeshes[i].Draw(shader);
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error::Assimp::" << importer.GetErrorString() << std::endl;
		return;
	}

	mDirectory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		mMeshes.push_back(ProcessMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// vertices
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// texture
		if (mesh->mTextureCoords[0]) // does the mesh have any texture coords?
		{
			glm::vec2 vector;
			vector.x = mesh->mTextureCoords[0][i].x;
			vector.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}

	// indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// material
	if (mesh->mMaterialIndex >= 0) // does the mesh have a material?
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		// check if this texture has already been loaded
		bool skip = false;
		for (int j = 0; j < mLoadedTextures.size(); j++)
		{
			if (std::strcmp(mLoadedTextures[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(mLoadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), mDirectory);
			texture.type = typeName;
			texture.path = std::string(str.C_Str());
			textures.push_back(texture);
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename(path);
	filename = directory + '/' + filename;

	return loadTexture(filename);
}