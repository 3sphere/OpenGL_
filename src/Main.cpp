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

// create a first-person camera
Camera camera(0.0f, 1.5f, 2.0f);

// per-frame time logic
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Game loop functions
void processInput(GLFWwindow* window);
void update();
void render(GLFWwindow* window);

// utility functions
unsigned int loadTexture(const std::string& path, bool alpha);
void bindTextureMaps(unsigned int map0, unsigned int map1);

// Shaders
std::map<std::string, Shader> shaderMap;

// Textures
std::map<std::string, unsigned int> textureMap;

// VAOs
std::map<std::string, unsigned int> vaoMap;

// Models
std::map<std::string, Model> modelMap;

int main()
{
	// Initialise GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	// Configure stencil testing for outline drawing
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Load shaders and set the uniforms that will not change each frame
	shaderMap["object"] = Shader("shaders/object_vs.txt", "shaders/object_fs.txt");
	shaderMap["light cube"] = Shader("shaders/object_vs.txt", "shaders/light_cube_fs.txt");
	shaderMap["object"].Use();
	shaderMap["object"].SetInt("material.diffuseMap", 0);
	shaderMap["object"].SetInt("material.specularMap", 1);
	shaderMap["object"].SetFloat("material.shininess", 64.0f);
	shaderMap["object"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["object"].SetFloat("pointLight.linear", 0.07f);
	shaderMap["object"].SetFloat("pointLight.quadratic", 0.017f);
	shaderMap["object"].SetVec3f("pointLight.ambient", 0.2f, 0.2f, 0.2f);
	shaderMap["object"].SetVec3f("pointLight.diffuse", 0.7f, 0.7f, 0.7f);
	shaderMap["object"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);

	// Load textures
	textureMap["cube_diffuse"] = loadTexture("textures/container2.png", true);
	textureMap["cube_specular"] = loadTexture("textures/container2_specular.png", true);
	textureMap["floor_diffuse"] = loadTexture("textures/wood_floor_diffuse.jpg", false);
	textureMap["floor_specular"] = loadTexture("textures/wood_floor_specular.jpg", false);
	textureMap["wall_diffuse"] = loadTexture("textures/wall_diffuse.jpg", false);
	textureMap["wall_specular"] = loadTexture("textures/wall_specular.jpg", false);

	// Set up VAOs
	// cube
	unsigned int cubeVAO, cubeVBO, cubeEBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BasicMeshes::Cube::vertices), BasicMeshes::Cube::vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BasicMeshes::Cube::indices), BasicMeshes::Cube::indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	vaoMap["cube"] = cubeVAO;
	// open cube with inverted normals
	unsigned int openCubeVAO, openCubeVBO, openCubeEBO;
	glGenVertexArrays(1, &openCubeVAO);
	glGenBuffers(1, &openCubeVBO);
	glGenBuffers(1, &openCubeEBO);
	glBindVertexArray(openCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, openCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BasicMeshes::OpenCubeInvertedNormals::vertices), BasicMeshes::OpenCubeInvertedNormals::vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openCubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BasicMeshes::OpenCubeInvertedNormals::indices), BasicMeshes::OpenCubeInvertedNormals::indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	vaoMap["open cube"] = openCubeVAO;
	// quad
	unsigned int quadVAO, quadVBO, quadEBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadEBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BasicMeshes::Quad::vertices), BasicMeshes::Quad::vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BasicMeshes::Quad::indices), BasicMeshes::Quad::indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	vaoMap["quad"] = quadVAO;
	
	// Load models
	modelMap["nanosuit"] = Model("models/nanosuit/nanosuit.obj");

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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 model(1.0f);

	// Render light source
	shaderMap["light cube"].Use();
	shaderMap["light cube"].SetMat4f("projection", projection);
	shaderMap["light cube"].SetMat4f("view", view);
	glm::vec3 lightCubePos(2.0f * cosf(glfwGetTime()), 2.0f, -2.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightCubePos);
	model = glm::scale(model, glm::vec3(0.1f));
	shaderMap["light cube"].SetMat4f("model", model);
	glBindVertexArray(vaoMap["cube"]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Render objects
	// 1. cubes
	shaderMap["object"].Use();
	shaderMap["object"].SetVec2f("textureScale", 1.0f, 1.0f);
	shaderMap["object"].SetMat4f("projection", projection);
	shaderMap["object"].SetMat4f("view", view);
	shaderMap["object"].SetVec3f("viewPos", camera.GetPosition());
	shaderMap["object"].SetVec3f("pointLight.position", lightCubePos);
	bindTextureMaps(textureMap["cube_diffuse"], textureMap["cube_specular"]);
	glBindVertexArray(vaoMap["cube"]);
	for (int i = -1; i < 2; i++)
	{
		glm::vec3 pos(i * 2.5f, 1.0f, -7.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		float angle = 50.0f * glfwGetTime();
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		shaderMap["object"].SetMat4f("model", model);
		
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	// 2. nanosuit model
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.5f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	shaderMap["object"].SetMat4f("model", model);
	modelMap["nanosuit"].Draw(shaderMap["object"]);
	// 3. floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shaderMap["object"].SetMat4f("model", model);
	shaderMap["object"].SetVec2f("textureScale", 10.0f, 10.0f);
	bindTextureMaps(textureMap["floor_diffuse"], textureMap["floor_specular"]);
	glBindVertexArray(vaoMap["quad"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// 4. walls
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::scale(model, glm::vec3(10.0f, 7.0f, 10.0f));
	shaderMap["object"].SetMat4f("model", model);
	shaderMap["object"].SetVec2f("textureScale", 5.0f, 5.0f);
	bindTextureMaps(textureMap["wall_diffuse"], textureMap["wall_specular"]);
	glBindVertexArray(vaoMap["open cube"]);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
}

unsigned int loadTexture(const std::string& path, bool alpha)
{
	unsigned int id;
	glGenTextures(1, &id);

	int width, height, numChannels;
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
	if (image)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		if(alpha)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image);
	}
	else
	{
		std::cout << "Failed to load texture at path: " << path << std::endl;
		stbi_image_free(image);
	}

	return id;
}

void bindTextureMaps(unsigned int map0, unsigned int map1)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, map0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, map1);
}