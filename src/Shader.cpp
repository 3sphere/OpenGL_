#include "Shader.h"
#include <fstream>
#include <sstream>
#include <glfw3.h>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
	// 1. Read vertex shader and fragment shader source code from file
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	std::ifstream geometryFile;
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	geometryFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files and create string streams
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		std::stringstream vertexStream, fragmentStream;
		// read file into streams
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		// close files
		vertexFile.close();
		fragmentFile.close();
		// convert streams to strings
		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();

		if (geometryPath != "")
		{
			geometryFile.open(geometryPath);
			std::stringstream geometryStream;
			geometryStream << geometryFile.rdbuf();
			geometryFile.close();
			geometryCode = geometryStream.str();
		}
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "Error::Shader::File not read" << std::endl;
	}

	// convert source code to c string
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// 2. Compile shaders
	unsigned int vertex, fragment;
	// vertex
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, NULL);
	glCompileShader(vertex);
	CheckCompilation(vertex, "Vertex");
	// fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, NULL);
	glCompileShader(fragment);
	CheckCompilation(fragment, "Fragment");
	// geometry (if present)
	unsigned int geometry;
	if (geometryPath != "")
	{
		const char* geometrySource = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometrySource, NULL);
		glCompileShader(geometry);
		CheckCompilation(geometry, "Geometry");
	}
	// complete shader program
	mID = glCreateProgram();
	glAttachShader(mID, vertex);
	glAttachShader(mID, fragment);
	if (geometryPath != "")
		glAttachShader(mID, geometry);
	glLinkProgram(mID);
	CheckCompilation(mID, "Program");
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != "")
		glDeleteShader(geometry);
}

void Shader::Use()
{
	glUseProgram(mID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
}

void Shader::SetVec2f(const std::string& name, float v1, float v2) const
{
	glUniform2f(glGetUniformLocation(mID, name.c_str()), v1, v2);
}

void Shader::SetVec2f(const std::string& name, const glm::vec2& vec) const
{
	glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &vec[0]);
}

void Shader::SetVec3f(const std::string& name, float v1, float v2, float v3) const
{
	glUniform3f(glGetUniformLocation(mID, name.c_str()), v1, v2, v3);
}

void Shader::SetVec3f(const std::string& name, const glm::vec3& vec) const
{
	glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, &vec[0]);
}

void Shader::SetVec4f(const std::string& name, float v1, float v2, float v3, float v4) const
{
	glUniform4f(glGetUniformLocation(mID, name.c_str()), v1, v2, v3, v4);
}

void Shader::SetVec4f(const std::string& name, const glm::vec4& vec) const
{
	glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &vec[0]);
}

void Shader::SetMat3f(const std::string& name, const glm::mat3& matrix) const
{
	glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat4f(const std::string& name, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::CheckCompilation(unsigned int id, std::string type)
{
	int success;
	char infoLog[512];
	for (auto& i : type)
		i = toupper(i);

	if (type != "PROGRAM")
	{
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(id, 512, NULL, infoLog);
			std::cout << "Error::Shader::" + type + "::Compilation failed\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cout << "Error::Shader::Program::Linking failed\n" << infoLog << std::endl;
		}
	}
}