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
unsigned int loadTexture(const std::string& path);
void bindTextureMaps(unsigned int map0, unsigned int map1);
inline float billboard(const glm::vec3& camPos, const glm::vec3& objPos);
unsigned int createFramebuffer(unsigned int width, unsigned int height);
unsigned int loadCubemap(std::vector<std::string> faces);

// Maps
std::map<std::string, Shader> shaderMap;
std::map<std::string, unsigned int> textureMap;
std::map<std::string, unsigned int> vaoMap;
std::map<std::string, Model> modelMap;
std::map<std::string, unsigned int> framebufferMap;

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
	shaderMap["skybox"] = Shader("shaders/skybox_vs.txt", "shaders/skybox_fs.txt");
	shaderMap["window"] = Shader("shaders/window_vs.txt", "shaders/window_fs.txt");
	shaderMap["object"].Use();
	shaderMap["object"].SetInt("material.texture_diffuse1", 0);
	shaderMap["object"].SetInt("material.texture_specular1", 1);
	shaderMap["object"].SetFloat("material.shininess", 64.0f);
	shaderMap["object"].SetFloat("pointLight.constant", 1.0f);
	shaderMap["object"].SetFloat("pointLight.linear", 0.027f);
	shaderMap["object"].SetFloat("pointLight.quadratic", 0.0028f);
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
	shaderMap["skybox"].Use();
	shaderMap["skybox"].SetInt("skybox", 0);

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
	// skybox
	std::vector<std::string> faces =
	{
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg",
	};
	textureMap["skybox"] = loadCubemap(faces);
	
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

	// Light source
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

	// Cubes
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
	shaderMap["transparency"].SetMat4f("projection", projection);
	shaderMap["transparency"].SetMat4f("view", view);
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
	shaderMap["window"].SetMat4f("projection", projection);
	shaderMap["window"].SetMat4f("view", view);
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
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else if (numChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glGenerateMipmap(GL_TEXTURE_2D);

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

inline float billboard(const glm::vec3& camPos, const glm::vec3& objPos)
{
	return atan2f(camPos.x - objPos.x, camPos.z - objPos.z);
}

unsigned int createFramebuffer(unsigned int width, unsigned int height)
{
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a texture for the colour attachment
	unsigned int texColourBuffer;
	glGenTextures(1, &texColourBuffer);
	glBindTexture(GL_TEXTURE_2D, texColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a renderbuffer for the depth and stencil attachments
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach the attachments to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColourBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// check if the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error::Framebuffer::Framebuffer is incomplete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return framebuffer;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, numChannels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char* image = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);
		if (image)
		{
			if(numChannels == 3)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			else if (numChannels == 4)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			stbi_image_free(image);
		}
		else
		{
			std::cout << "Failed to load cubemap texture at path: " << faces[i] << std::endl;
			stbi_image_free(image);
		}
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	return id;
}