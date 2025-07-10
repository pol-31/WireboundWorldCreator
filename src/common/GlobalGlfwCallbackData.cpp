#include "GlobalGlfwCallbackData.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "../common/TextRenderer.h"
#include "PickingFramebuffer.h"
#include "../modes/IUiMode.h"

void GlobalGlfwCallbackData::UpdateCursorPos() {
  glfwGetCursorPos(gWindow, &cursor_pos_.x, &cursor_pos_.y);
  cursor_pos_tex_norm_ = {
      (cursor_pos_.x / gWindowWidth) * 2.0f - 1.0f,
      ((gWindowHeight - cursor_pos_.y) / gWindowHeight) * 2.0f - 1.0f
  };
}

GLuint GlobalGlfwCallbackData::GetIdByMousePos() const {
  return picking_fbo->GetIdByMousePos(cursor_pos_);
}
