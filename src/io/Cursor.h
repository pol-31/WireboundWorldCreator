#ifndef WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_

#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../common/Texture.h"
#include "../common/Shader.h"
#include "../common/Details.h"

class Cursor {
 public:
  Cursor() = default;

  void SetModeDisabled();
  void SetModeEnabled();

 private:
  double disable_mode_last_x_ = 0.0f;
  double disable_mode_last_y_ = 0.0f;
};

#endif //WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_
