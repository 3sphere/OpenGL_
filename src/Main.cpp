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
std::map<std::string, unsigned int> vaoMap;
std::map<std::string, Model> modelMap;
std::map<std::string, unsigned int> framebufferMap;
std::map<std::string, unsigned int> uboMap;

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

	// Enable face culling
	glEnable(GL_CULL_FACE);

	// Load shaders and set the uniforms that will not change each frame
	shaderMap["object"] = Shader("shaders/object_vs.txt", "shaders/object_fs.txt");
	shaderMap["light cube"] = Shader("shaders/object_vs.txt", "shaders/light_cube_fs.txt");
	shaderMap["transparency"] = Shader("shaders/object_vs.txt", "shaders/transparency_fs.txt");
	shaderMap["window"] = Shader("shaders/window_vs.txt", "shaders/window_fs.txt");
	shaderMap["object"].Use();
	shaderMap["object"].SetInt("material.texture_diffuse1", 0);
	shaderMap["object"].SetInt("material.texture_specular1", 1);
	shaderMap["object"].SetFloat("material.shininess", 64.0f);
	shaderMap["object"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["object"].SetFloat("pointLight.linear", 0.07f);
	shaderMap["object"].SetFloat("pointLight.quadratic", 0.017f);
	shaderMap["object"].SetVec3f("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderMap["object"].SetVec3f("pointLight.diffuse", 0.8f, 0.66f, 0.41f);
	shaderMap["object"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
	shaderMap["transparency"].Use();
	shaderMap["transparency"].SetInt("material.texture_diffuse1", 0);
	shaderMap["transparency"].SetInt("material.texture_specular1", 1);
	shaderMap["transparency"].SetFloat("material.shininess", 64.0f);
	shaderMap["transparency"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["transparency"].SetFloat("pointLight.linear", 0.07f);
	shaderMap["transparency"].SetFloat("pointLight.quadratic", 0.017f);
	shaderMap["transparency"].SetVec3f("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderMap["transparency"].SetVec3f("pointLight.diffuse", 0.8f, 0.66f, 0.41f);
	shaderMap["transparency"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
	shaderMap["window"].Use();
	shaderMap["window"].SetInt("material.texture_diffuse1", 0);
	shaderMap["window"].SetInt("material.texture_specular1", 2);
	shaderMap["window"].SetFloat("material.shininess", 32.0f);
	shaderMap["window"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["window"].SetFloat("pointLight.linear", 0.07f);
	shaderMap["window"].SetFloat("pointLight.quadratic", 0.017f);
	shaderMap["window"].SetVec3f("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderMap["window"].SetVec3f("pointLight.diffuse", 0.8f, 0.66f, 0.41f);
	shaderMap["window"].SetVec3f("pointLight.specular", 1.0f, 1.0f, 1.0f);

	// Load textures
	stbi_set_flip_vertically_on_load(true);
	textureMap["plant_diffuse"] = loadTexture("textures/tree.png");
	textureMap["window"] = loadTexture("textures/window.png");
	stbi_set_flip_vertically_on_load(false);
	textureMap["cube_diffuse"] = loadTexture("textures/container2.png");
	textureMap["cube_specular"] = loadTexture("textures/container2_specular.png");
	textureMap["floor_diffuse"] = loadTexture("textures/wood_floor_diffuse.jpg");
	textureMap["floor_specular"] = loadTexture("textures/wood_floor_specular.jpg");
	textureMap["wall_diffuse"] = loadTexture("textures/wall_diffuse.jpg");
	textureMap["wall_specular"] = loadTexture("textures/wall_specular.jpg");
	textureMap["glass"] = loadTexture("textures/glass.png");
	std::vector<std::string> textures =
	{
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
	};
	textureMap["skybox"] = loadCubemap(textures);
	
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
	// cube with inverted normals
	unsigned int invertedCubeVAO, invertedCubeVBO, invertedCubeEBO;
	glGenVertexArrays(1, &invertedCubeVAO);
	glGenBuffers(1, &invertedCubeVBO);
	glGenBuffers(1, &invertedCubeEBO);
	glBindVertexArray(invertedCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, invertedCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BasicMeshes::CubeInvertedNormals::vertices), BasicMeshes::CubeInvertedNormals::vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, invertedCubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BasicMeshes::CubeInvertedNormals::indices), BasicMeshes::CubeInvertedNormals::indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	vaoMap["inside cube"] = invertedCubeVAO;
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
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMap["matrices"]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

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

	// Put the view matrix into the correct uniform buffer
	glm::mat4 view = camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, uboMap["matrices"]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 model(1.0f);
	glm::vec3 lightCubePos(2.0f * cosf(glfwGetTime()), 2.0f, -2.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, uboMap["lighting"]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4 * sizeof(float), glm::value_ptr(camera.GetPosition()));
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 4 * sizeof(float), glm::value_ptr(lightCubePos));

	// Light source
	shaderMap["light cube"].Use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightCubePos);
	model = glm::scale(model, glm::vec3(0.1f));
	shaderMap["light cube"].SetMat4f("model", model);
	glBindVertexArray(vaoMap["cube"]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Cubes
	shaderMap["object"].Use();
	shaderMap["object"].SetVec2f("textureScale", 1.0f, 1.0f);
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
	// Nanosuit model
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.5f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	shaderMap["object"].SetMat4f("model", model);
	modelMap["nanosuit"].Draw(shaderMap["object"]);
	// Floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shaderMap["object"].SetMat4f("model", model);
	shaderMap["object"].SetVec2f("textureScale", 10.0f, 10.0f);
	bindTextureMaps(textureMap["floor_diffuse"], textureMap["floor_specular"]);
	glBindVertexArray(vaoMap["quad"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// Walls and ceiling
	glFrontFace(GL_CW);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::scale(model, glm::vec3(10.0f, 7.0f, 10.0f));
	shaderMap["object"].SetMat4f("model", model);
	shaderMap["object"].SetVec2f("textureScale", 5.0f, 5.0f);
	bindTextureMaps(textureMap["wall_diffuse"], textureMap["wall_specular"]);
	glBindVertexArray(vaoMap["inside cube"]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
	bindTextureMaps(textureMap["plant_diffuse"], 0);
	glBindVertexArray(vaoMap["quad"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// second
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.9f, -7.0f));
	model = glm::rotate(model, billboard(camera.GetPosition(), glm::vec3(-4.0f, 0.9f, -7.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
	shaderMap["transparency"].SetMat4f("model", model);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	// Glass pane
	shaderMap["transparency"].SetBool("specular", true);
	shaderMap["transparency"].SetVec3f("material.specular", 0.5f, 0.5f, 0.5f);
	shaderMap["transparency"].SetFloat("material.shininess", 32.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, -6.0f));
	model = glm::scale(model, glm::vec3(10.0f, 2.0f, 1.0f));
	shaderMap["transparency"].SetMat4f("model", model);
	bindTextureMaps(textureMap["glass"], 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);

	// Windows
	shaderMap["window"].Use();
	shaderMap["window"].SetBool("specular", false);
	shaderMap["window"].SetVec3f("viewPos", camera.GetPosition());
	shaderMap["window"].SetVec3f("pointLight.position", lightCubePos);
	shaderMap["window"].SetInt("material.texture_diffuse1", 0);
	shaderMap["window"].SetInt("skybox", 1);
	// first
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.95f, 1.5f, -3.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.0f));
	shaderMap["window"].SetMat4f("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap["window"]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureMap["skybox"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// second
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.95f, 1.5f, -3.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.0f));
	shaderMap["window"].SetMat4f("model", model);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	glfwSwapBuffers(window);
}