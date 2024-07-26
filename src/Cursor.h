#ifndef WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_

#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Texture.h"
#include "Shader.h"
#include "Details.h"

class Cursor {
 public:
  Cursor(std::string_view path) : tex_(path) {}

  void SetModeDisabled(GLFWwindow* window);
  void SetModeEnabled(GLFWwindow* window);

  void SwitchMode(GLFWwindow* window);

  void Render(const Shader& shader, GLuint vao_id, GLuint vbo_id);
 private:
  bool enabled_{true};
  Texture tex_;

  double disable_mode_last_x_ = 0.0f;
  double disable_mode_last_y_ = 0.0f;
};

#endif //WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_
