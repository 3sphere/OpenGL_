#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad\glad.h>
#include <glfw3.h>
#include <iostream>
#include "Callback.h"
#include "Camera.h"
#include "Shader.h"
#include <map>
#include <vector>
#include "Mesh.h"
#include "Model.h"
#include "BasicMeshes.h"
#include "Utility.h"
#include "BasicMesh.h"

// create a first-person camera
Camera camera(0.0f, 1.5f, 2.0f);

// per-frame time logic
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Game loop functions
void processInput(GLFWwindow* window);
void update();
void render(GLFWwindow* window);

// Maps
std::map<std::string, Shader> shaderMap;
std::map<std::string, unsigned int> textureMap;
std::map<std::string, Model> modelMap;
std::map<std::string, unsigned int> framebufferMap;
std::map<std::string, unsigned int> uboMap;

std::map<std::string, BasicMesh> meshMap;

// uniforms
float heightScale = 0.1f;

int main()
{
	// Initialise GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create window
	GLFWwindow* window = glfwCreateWindow(1024, 720, "OpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set callback functions
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Capture cursor within window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	// Set default viewport
	glViewport(0, 0, 1024, 720);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Enable face culling
	glEnable(GL_CULL_FACE);

	// MSAA
	glEnable(GL_MULTISAMPLE);

	// Load shaders and set the uniforms that will not change each frame
	shaderMap["object"] = Shader("shaders/object_vs.txt", "shaders/object_fs.txt");
	shaderMap["light cube"] = Shader("shaders/object_vs.txt", "shaders/light_cube_fs.txt");
	shaderMap["transparency"] = Shader("shaders/object_vs.txt", "shaders/transparency_fs.txt");
	shaderMap["window"] = Shader("shaders/window_vs.txt", "shaders/window_fs.txt");
	shaderMap["depth"] = Shader("shaders/depth_map_vs.txt", "shaders/depth_map_fs.txt", "shaders/depth_map_gs.txt");

	shaderMap["object"].Use();
	shaderMap["object"].SetInt("depthMap", 4);
	shaderMap["object"].SetFloat("material.shininess", 32.0f);
	shaderMap["object"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["object"].SetFloat("pointLight.linear", 0.22f);
	shaderMap["object"].SetFloat("pointLight.quadratic", 0.20f);
	shaderMap["object"].SetVec3f("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderMap["object"].SetVec3f("pointLight.diffuse", 0.96f, 0.75f, 0.26f);
	shaderMap["object"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
	shaderMap["object"].SetFloat("heightScale", 0.1f);
	shaderMap["transparency"].Use();
	shaderMap["transparency"].SetFloat("material.shininess", 32.0f);
	shaderMap["transparency"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["transparency"].SetFloat("pointLight.linear", 0.22f);
	shaderMap["transparency"].SetFloat("pointLight.quadratic", 0.20f);
	shaderMap["transparency"].SetVec3f("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderMap["transparency"].SetVec3f("pointLight.diffuse", 0.96f, 0.75f, 0.26f);
	shaderMap["transparency"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
	shaderMap["window"].Use();
	shaderMap["window"].SetFloat("material.shininess", 32.0f);
	shaderMap["window"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["window"].SetFloat("pointLight.linear", 0.22f);
	shaderMap["window"].SetFloat("pointLight.quadratic", 0.20f);
	shaderMap["window"].SetVec3f("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderMap["window"].SetVec3f("pointLight.diffuse", 0.96f, 0.75f, 0.26f);
	shaderMap["window"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);

	// Load textures
	std::vector<std::string> skyboxTextures =
	{
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
	};
	textureMap["skybox"] = loadCubemap(skyboxTextures);

	std::vector<Texture> wallTextures =
	{
		{loadTextureSRGB("textures/wall_diffuse.jpg"), "texture_diffuse"},
		{loadTexture("textures/wall_specular.jpg"), "texture_specular"},
		{loadTexture("textures/wall_normal.jpg"), "texture_normal"}
	};

	std::vector<Texture> floorTextures =
	{
		{loadTextureSRGB("textures/wood_floor_diffuse.jpg"), "texture_diffuse"},
		{loadTexture("textures/wood_floor_specular.jpg"), "texture_specular"},
		{loadTexture("textures/wood_floor_normal.jpg"), "texture_normal"}
	};

	std::vector<Texture> glassPaneTextures =
	{
		{loadTextureSRGB("textures/glass.png"), "texture_diffuse"},
		{0, "texture_specular"}
	};

	std::vector<Texture> plantTextures =
	{
		{loadTextureSRGB("textures/tree.png"), "texture_diffuse"},
		{0, "texture_specular"}
	};

	std::vector<Texture> windowTextures =
	{
		{loadTextureSRGB("textures/window.png"), "texture_diffuse"},
		{0, "texture_specular"}
	};

	std::vector<Texture> brickTextures =
	{
		{loadTextureSRGB("textures/bricks.jpg"), "texture_diffuse"},
		{0, "texture_specular"},
		{loadTexture("textures/toy_box_normal.png"), "texture_normal"},
		{loadTexture("textures/toy_box_displacement.png"), "texture_displacement"}
	};

	std::vector<Texture> crateTextures =
	{
		{loadTextureSRGB("textures/wood2_diffuse.jpg"), "texture_diffuse"},
		{loadTexture("textures/wood2_specular.jpg"), "texture_specular"},
		{loadTexture("textures/wood2_normal.jpg"), "texture_normal"},
		{loadTexture("textures/wood2_displacement_inverted.png"), "texture_displacement"}
	};

	std::vector<Texture> metalTextures =
	{
		{loadTextureSRGB("textures/metal_diffuse.jpg"), "texture_diffuse"},
		{loadTexture("textures/metal_specular.jpg"), "texture_specular"},
		{loadTexture("textures/metal_normal.jpg"), "texture_normal"},
		{loadTexture("textures/metal_displacement_inverted.png"), "texture_displacement"}
	};

	// Create basic meshes
	meshMap["cube"] = BasicMesh(BasicMeshes::Cube::Vertices, BasicMeshes::Cube::Indices);
	meshMap["plant"] = BasicMesh(BasicMeshes::Quad::Vertices, BasicMeshes::Quad::Indices, plantTextures);
	meshMap["glass pane"] = BasicMesh(BasicMeshes::Quad::Vertices, BasicMeshes::Quad::Indices, glassPaneTextures);
	meshMap["window"] = BasicMesh(BasicMeshes::Quad::Vertices, BasicMeshes::Quad::Indices, windowTextures);
	meshMap["floor"] = BasicMesh(BasicMeshes::Quad::Vertices, BasicMeshes::Quad::Indices, floorTextures);
	meshMap["box"] = BasicMesh(BasicMeshes::Cube::Vertices, BasicMeshes::Cube::Indices, crateTextures);
	meshMap["inverted cube"] = BasicMesh(BasicMeshes::CubeInvertedNormals::Vertices, BasicMeshes::CubeInvertedNormals::Indices, wallTextures);
	meshMap["parallax cube"] = BasicMesh(BasicMeshes::Cube::Vertices, BasicMeshes::Cube::Indices, metalTextures);

	// Load models
	modelMap["nanosuit"] = Model("models/nanosuit/nanosuit.obj");

	// Uniform buffer objects
	// 1. "Matrices" uniform block
	// Set the uniform block of the vertex shaders equal to binding point 0
	bindUniformBlockToPoint(shaderMap["object"], "Matrices", 0);
	bindUniformBlockToPoint(shaderMap["light cube"], "Matrices", 0);
	bindUniformBlockToPoint(shaderMap["window"], "Matrices", 0);
	bindUniformBlockToPoint(shaderMap["transparency"], "Matrices", 0);
	// Create uniform buffer object and bind it to binding point 0
	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	uboMap["matrices"] = uboMatrices;
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMap["matrices"], 0, 2 * sizeof(glm::mat4));
	// Put the projection matrix into the uniform buffer
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMap["matrices"]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Depth cubemap setup
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	framebufferMap["depth"] = depthMapFBO;
	textureMap["depth"] = depthCubemap;

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		update();
		render(window);
	}

	// Clean up resources and exit
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	camera.ProcessInput(window);
}

void update()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	if (deltaTime > 0.05f)
		deltaTime = 0.05f;
	lastFrame = currentFrame;

	camera.Update(deltaTime);
}

void render(GLFWwindow* window)
{
	glm::vec3 lightCubePos(2.0f * cosf(glfwGetTime()), 2.0f, -2.0f);
	glm::mat4 model(1.0f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// First render pass: render to depth map from light's perspective
	float nearPlane = 1.0f, farPlane = 25.0f, aspect = 1.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightCubePos, lightCubePos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightCubePos, lightCubePos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightCubePos, lightCubePos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightCubePos, lightCubePos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightCubePos, lightCubePos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightCubePos, lightCubePos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferMap["depth"]);
	glClear(GL_DEPTH_BUFFER_BIT);
	shaderMap["depth"].Use();
	for (int i = 0; i < 6; ++i)
		shaderMap["depth"].SetMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	shaderMap["depth"].SetFloat("farPlane", farPlane);
	shaderMap["depth"].SetVec3f("lightPos", lightCubePos);
	// Rotating boxes
	for (int i = -1; i < 2; i++)
	{
		glm::vec3 pos(i * 2.5f, 1.0f, -7.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		float angle = 50.0f * glfwGetTime();
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		shaderMap["depth"].SetMat4f("model", model);

		meshMap["box"].Draw(shaderMap["depth"]);
	}
	// parallax cube
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.5f, -2.0f));
	shaderMap["depth"].SetMat4f("model", model);
	meshMap["parallax cube"].Draw(shaderMap["depth"]);
	// Nanosuit model
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.5f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	shaderMap["depth"].SetMat4f("model", model);
	modelMap["nanosuit"].Draw(shaderMap["depth"]);
	// Plants
	// first
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, 0.9f, -7.0f));
	model = glm::rotate(model, billboard(camera.GetPosition(), glm::vec3(4.0f, 0.9f, -7.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 2.0f, 1.0f));
	shaderMap["depth"].SetMat4f("model", model);
	meshMap["plant"].Draw(shaderMap["depth"]);
	// second
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.9f, -7.0f));
	model = glm::rotate(model, billboard(camera.GetPosition(), glm::vec3(-4.0f, 0.9f, -7.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 2.0f, 1.0f));
	shaderMap["depth"].SetMat4f("model", model);
	meshMap["plant"].Draw(shaderMap["depth"]);


	// Second render pass: render the scene as normal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1024, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 view = camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, uboMap["matrices"]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Light source
	shaderMap["light cube"].Use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightCubePos);
	model = glm::scale(model, glm::vec3(0.1f));
	shaderMap["light cube"].SetMat4f("model", model);
	meshMap["cube"].Draw(shaderMap["light cube"]);

	// Rotating boxes
	shaderMap["object"].Use();
	shaderMap["object"].SetFloat("heightScale", heightScale);
	shaderMap["object"].SetBool("parallaxMapping", true);
	shaderMap["object"].SetBool("normalMapping", true);
	shaderMap["object"].SetFloat("farPlane", farPlane);
	shaderMap["object"].SetVec2f("textureScale", 1.0f, 1.0f);
	shaderMap["object"].SetVec3f("viewPos", camera.GetPosition());
	shaderMap["object"].SetVec3f("lightPos", lightCubePos);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureMap["depth"]);
	for (int i = -1; i < 2; i++)
	{
		glm::vec3 pos(i * 2.5f, 1.0f, -7.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		float angle = 50.0f * glfwGetTime();
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		shaderMap["object"].SetMat4f("model", model);

		meshMap["box"].Draw(shaderMap["object"]);
	}
	// parallax cube
	shaderMap["object"].SetBool("normalMapping", true);
	shaderMap["object"].SetBool("parallaxMapping", true);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.5f, -2.0f));
	shaderMap["object"].SetMat4f("model", model);
	meshMap["parallax cube"].Draw(shaderMap["object"]);
	// Nanosuit model
	shaderMap["object"].SetBool("normalMapping", false);
	shaderMap["object"].SetBool("parallaxMapping", false);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.5f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	shaderMap["object"].SetMat4f("model", model);
	modelMap["nanosuit"].Draw(shaderMap["object"]);
	// Floor
	shaderMap["object"].SetBool("normalMapping", true);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shaderMap["object"].SetMat4f("model", model);
	shaderMap["object"].SetVec2f("textureScale", 10.0f, 10.0f);
	meshMap["floor"].Draw(shaderMap["object"]);
	// Walls and ceiling
	glFrontFace(GL_CW);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::scale(model, glm::vec3(10.0f, 7.0f, 10.0f));
	shaderMap["object"].SetMat4f("model", model);
	shaderMap["object"].SetVec2f("textureScale", 5.0f, 5.0f);
	meshMap["inverted cube"].Draw(shaderMap["object"]);
	glFrontFace(GL_CCW);

	// Plants
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shaderMap["transparency"].Use();
	shaderMap["transparency"].SetBool("specular", false);
	shaderMap["transparency"].SetVec2f("textureScale", 1.0f, 1.0f);
	shaderMap["transparency"].SetVec3f("viewPos", camera.GetPosition());
	shaderMap["transparency"].SetVec3f("pointLight.position", lightCubePos);
	// first
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, 0.9f, -7.0f));
	model = glm::rotate(model, billboard(camera.GetPosition(), glm::vec3(4.0f, 0.9f, -7.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
	shaderMap["transparency"].SetMat4f("model", model);
	meshMap["plant"].Draw(shaderMap["transparency"]);
	// second
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.9f, -7.0f));
	model = glm::rotate(model, billboard(camera.GetPosition(), glm::vec3(-4.0f, 0.9f, -7.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
	shaderMap["transparency"].SetMat4f("model", model);
	meshMap["plant"].Draw(shaderMap["transparency"]);


	// Glass pane
	shaderMap["transparency"].SetBool("specular", true);
	shaderMap["transparency"].SetVec3f("material.specular", 0.5f, 0.5f, 0.5f);
	shaderMap["transparency"].SetFloat("material.shininess", 32.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, -6.0f));
	model = glm::scale(model, glm::vec3(10.0f, 2.0f, 1.0f));
	shaderMap["transparency"].SetMat4f("model", model);
	meshMap["glass pane"].Draw(shaderMap["transparency"]);
	glDisable(GL_BLEND);

	// Windows
	shaderMap["window"].Use();
	shaderMap["window"].SetBool("specular", false);
	shaderMap["window"].SetVec3f("viewPos", camera.GetPosition());
	shaderMap["window"].SetVec3f("pointLight.position", lightCubePos);
	shaderMap["window"].SetInt("skybox", 2);
	// first
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.95f, 1.5f, -3.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.0f));
	shaderMap["window"].SetMat4f("model", model);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureMap["skybox"]);
	meshMap["window"].Draw(shaderMap["window"]);
	// second
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.95f, 1.5f, -3.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.0f));
	shaderMap["window"].SetMat4f("model", model);
	meshMap["window"].Draw(shaderMap["window"]);

	glfwSwapBuffers(window);
}