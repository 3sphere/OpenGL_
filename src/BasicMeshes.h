#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "Mesh.h"

namespace BasicMeshes
{
	namespace Quad
	{
		std::vector<Vertex> Vertices = 
		{
			Vertex{glm::vec3(-0.5f, -0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f, -0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f,  0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  1.0f)}
		};

		std::vector<unsigned int> Indices =
		{
			0, 1, 2,
			2, 3, 0
		};

		float vertices[] =
		{
			// position           // normals		   // texture coords
			-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f
		};

		unsigned int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};
	}

	namespace Cube
	{
		std::vector<Vertex> Vertices = 
		{
			// front face
			Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  1.0f)},
			// rear face
			Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3 (0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  1.0f)},
			// left face
			Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f)},
			// right face
			Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f)},
			// top face
			Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  1.0f)},
			// bottom face
			Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  1.0f)}
		};

		std::vector<unsigned int> Indices = 
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

		float vertices[] =
		{
			// front face
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			// rear face
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			// left face
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			// right face
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			// top face
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			// bottom face
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f

		};

		unsigned int indices[] =
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
	}

	namespace CubeInvertedNormals
	{
		std::vector<Vertex> Vertices =
		{
			// front face
			Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  1.0f)},
			// rear face
			Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f,  1.0f)},
			// left face
			Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f)},
			// right face
			Vertex{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f)},
			// top face
			Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  1.0f)},
			// bottom face
			Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec2(0.0f,  0.0f)},
			Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec2(1.0f,  0.0f)},
			Vertex{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec2(1.0f,  1.0f)},
			Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec2(0.0f,  1.0f)}
		};

		std::vector<unsigned int> Indices =
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

		float vertices[] =
		{
			 // front face
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			 // rear face
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			 // left face
			-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 // right face
			 0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 // top face
			-0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 // bottom face
			-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

		};

		unsigned int indices[] =
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
	}

	namespace OpenCubeInvertedNormals
	{
		float vertices[] =
		{
			// front face
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			// rear face
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			// left face
			-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			// right face
			 0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			// bottom face
			-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		unsigned int indices[] =
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
			// bottom face
			16, 17, 18,
			18, 19, 16,
		};
	}
}