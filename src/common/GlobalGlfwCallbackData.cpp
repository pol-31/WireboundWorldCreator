#include "GlobalGlfwCallbackData.h"

#include "../modes/IEditMode.h"
#include "../common/TextRenderer.h"

void GlobalGlfwCallbackData::UpdateCursorPos() {
  glfwGetCursorPos(gWindow, &cursor_pos_.x, &cursor_pos_.y);
  cursor_pos_tex_norm_ = {
      (cursor_pos_.x / gWindowWidth) * 2.0f - 1.0f,
      ((gWindowHeight - cursor_pos_.y) / gWindowHeight) * 2.0f - 1.0f
  };
}

GlobalGlfwCallbackData::GlobalGlfwCallbackData(
    CameraHandler& camera/*, Cursor& cursor*/,
    TileRenderer& tile_renderer, IEditMode*& cur_mode,
    Menu& menu, PickingFramebuffer& picking_fbo,
    debug::UiDebugger& ui_debugger,
    SharedResources* shared_resources,
    TextRenderer* text_renderer)
    : camera_(camera),
      //        cursor_(cursor),
      tile_renderer_(tile_renderer),
      cur_mode_(cur_mode),
      menu_(menu),
      picking_fbo_(picking_fbo),
      ui_debugger_(ui_debugger),
      shared_resources_(shared_resources),
      text_renderer_(text_renderer) {}

GLuint GlobalGlfwCallbackData::GetIdByMousePos() const {
  return picking_fbo_.GetIdByMousePos(cursor_pos_);
}

void GlobalGlfwCallbackData::StartCharInput(FixedSizeQueue<char, 64>* name) {
  glfwSetCharCallback(gWindow, TextRendererCharCallback);
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetKeyCallback(gWindow, nullptr);
  glfwSetMouseButtonCallback(gWindow, TextRendererMouseButtonCallback);
  text_renderer_->StartInput(name);
}

void GlobalGlfwCallbackData::StopCharInput() {
  glfwSetCharCallback(gWindow, nullptr);
  cur_mode_->BindCallbacks();
  text_renderer_->StopInput();
}
