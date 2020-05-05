#pragma once
#include <string>
#include <glm\glm.hpp>
#include <vector>
#include "Shader.h"

unsigned int loadTexture(const std::string& path);
unsigned int loadTextureSRGB(const std::string& path);
void bindTextureMaps(unsigned int map0, unsigned int map1);
void bindTextureMaps(unsigned int map0, unsigned int map1, unsigned int map2);
unsigned int createFramebuffer(unsigned int width, unsigned int height);
unsigned int loadCubemap(std::vector<std::string> faces);
inline float billboard(const glm::vec3& camPos, const glm::vec3& objPos) { return atan2f(camPos.x - objPos.x, camPos.z - objPos.z); }
void bindUniformBlockToPoint(const Shader& shader, const std::string& blockName, unsigned int bindingPoint);