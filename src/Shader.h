#pragma once
#include <glad\glad.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader() = default;
	Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
	void Use();
	unsigned int GetID() const { return mID; }

	// uniform settings functions
	void SetBool(const std::string& name, bool value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetInt(const std::string& name, int value) const;
	void SetVec2f(const std::string& name, float v1, float v2) const;
	void SetVec2f(const std::string& name, const glm::vec2& vec) const;
	void SetVec3f(const std::string& name, float v1, float v2, float v3) const;
	void SetVec3f(const std::string& name, const glm::vec3& vec) const;
	void SetVec4f(const std::string& name, float v1, float v2, float v3, float v4) const;
	void SetVec4f(const std::string& name, const glm::vec4& vec) const;
	void SetMat3f(const std::string& name, const glm::mat3& matrix) const;
	void SetMat4f(const std::string& name, const glm::mat4& matrix) const;

private:
	void CheckCompilation(unsigned int id, std::string type);

	unsigned int mID;
};