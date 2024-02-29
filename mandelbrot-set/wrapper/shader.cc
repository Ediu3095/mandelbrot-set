#include "mandelbrot-set/wrapper/shader.h"

#include <glad/gl.h>

#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace opengl {

Shader::Shader(const std::filesystem::path &vertexPath, const std::filesystem::path &fragmentPath) {
  // 1. Retrieve the vertex/fragment source code from filePath
  std::string vertex_code, fragment_code;
  std::ifstream vertex_fstream, fragment_fstream;

  vertex_fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragment_fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vertex_fstream.open(vertexPath);
    fragment_fstream.open(fragmentPath);

    std::stringstream vertex_sstream, fragment_sstream;
    vertex_sstream << vertex_fstream.rdbuf();
    fragment_sstream << fragment_fstream.rdbuf();

    vertex_fstream.close();
    fragment_fstream.close();

    vertex_code = vertex_sstream.str();
    fragment_code = fragment_sstream.str();
  } catch (const std::ifstream::failure &ex) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    return;
  }

  const char *vertex_ccode = vertex_code.c_str();
  const char *fragment_ccode = fragment_code.c_str();

  // 2. Compile shaders
  uint32_t vertex_id, fragment_id;
  int32_t success, info_log_length;

  // Vertex shader
  vertex_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_id, 1, &vertex_ccode, NULL);
  glCompileShader(vertex_id);

  glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderiv(vertex_id, GL_INFO_LOG_LENGTH, &info_log_length);
    std::vector<char> info_log(info_log_length + 1);
    glGetShaderInfoLog(vertex_id, info_log_length, NULL, &info_log[0]);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
    std::cerr << &info_log[0] << std::endl;
    return;
  }

  // Fragment shader
  fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_id, 1, &fragment_ccode, NULL);
  glCompileShader(fragment_id);

  glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderiv(fragment_id, GL_INFO_LOG_LENGTH, &info_log_length);
    std::vector<char> info_log(info_log_length + 1);
    glGetShaderInfoLog(fragment_id, info_log_length, NULL, &info_log[0]);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
    std::cerr << &info_log[0] << std::endl;
    return;
  }

  // Shader program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex_id);
  glAttachShader(id_, fragment_id);
  glLinkProgram(id_);

  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &info_log_length);
    std::vector<char> info_log(info_log_length + 1);
    glGetProgramInfoLog(id_, info_log_length, NULL, &info_log[0]);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl;
    std::cerr << &info_log[0] << std::endl;
    return;
  }

  // Detach and delete the shaders as they're now linked into our program and no longer necessary
  glDetachShader(id_, vertex_id);
  glDetachShader(id_, fragment_id);

  glDeleteShader(vertex_id);
  glDeleteShader(fragment_id);
}

void Shader::Use() {
  glUseProgram(id_);
}

template <>
void Shader::SetUniform<glm::mat4>(const std::string &name, const glm::mat4 &value) const {
  GLuint location = glGetUniformLocation(id_, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
};

template <>
void Shader::SetUniform<glm::dvec4>(const std::string &name, const glm::dvec4 &value) const {
  GLuint location = glGetUniformLocation(id_, name.c_str());
  glUniform4dv(location, 1, &value[0]);
};

template <>
void Shader::SetUniform<float>(const std::string &name, const float &value) const {
  GLuint location = glGetUniformLocation(id_, name.c_str());
  glUniform1f(location, value);
};

template <>
void Shader::SetUniform<int>(const std::string &name, const int &value) const {
  GLuint location = glGetUniformLocation(id_, name.c_str());
  glUniform1i(location, value);
};

};  // namespace opengl
