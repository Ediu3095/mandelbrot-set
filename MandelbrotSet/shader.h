#ifndef SHADER_H
#define SHADER_H

#include "GL/glew.h"
#include "glm/glm.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// Definition
class Shader
{
public:
	// The constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);

	// Activate the shader
	void use();

	// Uniform functions
	void setMatrix4fv(const std::string& name, const glm::mat4& value) const;
	void setVector4dv(const std::string& name, const glm::dvec4& value) const;
	void setFloat(const std::string& name, const float& value) const;

private:
	// The program ID
	GLuint ID;
};

// Implementation
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		// Read files's bufffer contents into streams;
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close file handlers
		vShaderFile.close();
		fShaderFile.close();

		// Convert streams to strings
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure ex)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		return;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. Compile shaders
	GLuint vShaderID, fShaderID;
	GLint success;
	int infoLogLength;

	// Vertex shader
	vShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShaderID, 1, &vShaderCode, NULL);
	glCompileShader(vShaderID);

	// Check for errors
	glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderiv(vShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> infoLog(infoLogLength + 1);
		glGetShaderInfoLog(vShaderID, infoLogLength, NULL, &infoLog[0]);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
		std::cerr << &infoLog[0] << std::endl;
		return;
	}

	// Fragment shader
	fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShaderID, 1, &fShaderCode, NULL);
	glCompileShader(fShaderID);

	// Check for errors
	glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderiv(fShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> infoLog(infoLogLength + 1);
		glGetShaderInfoLog(fShaderID, infoLogLength, NULL, &infoLog[0]);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
		std::cerr << &infoLog[0] << std::endl;
		return;
	}

	// Shader program
	ID = glCreateProgram();
	glAttachShader(ID, vShaderID);
	glAttachShader(ID, fShaderID);
	glLinkProgram(ID);

	// Check for errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> infoLog(infoLogLength + 1);
		glGetProgramInfoLog(ID, infoLogLength, NULL, &infoLog[0]);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl;
		std::cerr << &infoLog[0] << std::endl;
		return;
	}

	// Detach and delete the shaders as they're now linked into our program and no longer necessary
	glDetachShader(ID, vShaderID);
	glDetachShader(ID, fShaderID);

	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setMatrix4fv(const std::string& name, const glm::mat4& value) const
{
	GLuint location = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setVector4dv(const std::string& name, const glm::dvec4& value) const
{
	GLuint location = glGetUniformLocation(ID, name.c_str());
	glUniform4dv(location, 1, &value[0]);
}

void Shader::setFloat(const std::string& name, const float& value) const
{
	GLuint location = glGetUniformLocation(ID, name.c_str());
	glUniform1f(location, value);
}

#endif
