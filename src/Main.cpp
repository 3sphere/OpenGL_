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

// create a first-person camera
Camera camera(0.0f, 0.0f, 3.0f);

// per-frame time logic
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Game loop functions
void processInput(GLFWwindow* window);
void update();
void render(GLFWwindow* window);

// utility functions
unsigned int loadTexture(const std::string& path);

// Shaders
std::map<std::string, Shader> shaderMap;

// Textures
std::map<std::string, Texture> textureMap;

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

	// Load shaders and set the uniforms that will not change each frame
	shaderMap["object cube"] = Shader("shaders/cube_vs.txt", "shaders/objectCube_fs.txt");
	shaderMap["light cube"] = Shader("shaders/cube_vs.txt", "shaders/lightCube_fs.txt");
	shaderMap["object cube"].Use();
	shaderMap["object cube"].SetInt("material.diffuseMap", 0);
	shaderMap["object cube"].SetInt("material.specularMap", 1);
	shaderMap["object cube"].SetFloat("material.shininess", 64.0f);
	shaderMap["object cube"].SetVec3f("dirLight.direction", 0.0f, -1.0f, -1.0f);
	shaderMap["object cube"].SetVec3f("dirLight.ambient", 0.2f, 0.2f, 0.2f);
	shaderMap["object cube"].SetVec3f("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
	shaderMap["object cube"].SetVec3f("dirLight.specular", 1.0f, 1.0f, 1.0f);
	shaderMap["object cube"].SetVec3f("pointLight.position", 1.0f, 1.0f, 1.0f);
	shaderMap["object cube"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["object cube"].SetFloat("pointLight.linear", 0.09f);
	shaderMap["object cube"].SetFloat("pointLight.quadratic", 0.032f);
	shaderMap["object cube"].SetVec3f("pointLight.ambient", 0.1f, 0.1f, 0.1f);
	shaderMap["object cube"].SetVec3f("pointLight.diffuse", 0.7f, 0.7f, 0.7f);
	shaderMap["object cube"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);

	// Load textures
	textureMap["ceramic_diffuse"] = { loadTexture("textures/ceramic_diffuse.jpg"), "texture_diffuse" };
	textureMap["ceramic_specular"] = { loadTexture("textures/ceramic_specular.jpg"), "texture_specular" };

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 model(1.0f);

	// Render light cubes
	shaderMap["light cube"].Use();
	shaderMap["light cube"].SetMat4f("projection", projection);
	shaderMap["light cube"].SetMat4f("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.2f));
	shaderMap["light cube"].SetMat4f("model", model);
	Mesh lightCube(Cube::vertices, Cube::indices, {});
	lightCube.Draw(shaderMap["light cube"]);

	// Render object cubes
	shaderMap["object cube"].Use();
	shaderMap["object cube"].SetMat4f("projection", projection);
	shaderMap["object cube"].SetMat4f("view", view);
	shaderMap["object cube"].SetVec3f("viewPos", camera.GetPosition());
	std::vector<Texture> textures =
	{
		textureMap["ceramic_diffuse"], textureMap["ceramic_specular"]
	};
	Mesh objectCube(Cube::vertices, Cube::indices, textures);
	for (int i = 0; i < 3; i++)
	{
		glm::vec3 pos(0.0f, 0.0f, -i * 5.0f + 0.5f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		shaderMap["object cube"].SetMat4f("model", model);
		objectCube.Draw(shaderMap["object cube"]);
	}

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 2.0f, -5.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	shaderMap["object cube"].SetMat4f("model", model);
	modelMap["nanosuit"].Draw(shaderMap["object cube"]);

	glfwSwapBuffers(window);
}

unsigned int loadTexture(const std::string& path)
{
	unsigned int id;
	glGenTextures(1, &id);

	int width, height, numChannels;
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
	if (image)
	{
		glBindTexture(GL_TEXTURE_2D, id);

		if (numChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if(numChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture at path: " << path << std::endl;
	}
	stbi_image_free(image);

	return id;
}