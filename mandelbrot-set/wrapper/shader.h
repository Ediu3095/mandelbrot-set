#ifndef MANDELBROT_SET_WRAPPER_SHADER_H_
#define MANDELBROT_SET_WRAPPER_SHADER_H_

#include <glad/gl.h>

#include <string>
#include <filesystem>

#include <glm/glm.hpp>

namespace opengl {

class Shader {
 public:
  Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path);

  void Use();

  template <typename T>
  void SetUniform(const std::string &name, const T &value) const;

 private:
  GLuint id_;
};

};  // namespace opengl

#endif  // MANDELBROT_SET_WRAPPER_SHADER_H_
