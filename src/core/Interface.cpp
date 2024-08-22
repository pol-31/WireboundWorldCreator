#include "Interface.h"

Interface::Interface(const Paths& paths, Camera& camera,
                     Cursor& cursor, Map& map)
    : tile_renderer_(paths, visibility_, map),
      text_renderer_(paths),
      picking_fbo_(),
      shared_resources_(paths, tile_renderer_.GetTile(), tile_renderer_, global_data_),
      terrain_(shared_resources_),
      water_(shared_resources_, paths),
      roads_(shared_resources_),
      fences_(shared_resources_),
      biomes_(shared_resources_),
      objects_(shared_resources_),
      placement_(shared_resources_, paths),
      tiles_(shared_resources_),
      global_data_(camera, cursor, tile_renderer_,
                   cur_mode_, menu_, picking_fbo_),
      menu_(shared_resources_, text_renderer_, visibility_, &terrain_,
            &water_, &roads_, &fences_, &placement_, &objects_, &biomes_,
            &tiles_, cur_mode_) {
  Init();
}

void Interface::Render() {
  /// tile_renderer_ render terrain, water, fences, etc... based on enabled
  /// vision modes, menu_.RenderEditMode() render mode ui and selected graphs,
  /// points and internally calls tile_renderer_ to render terrain like this:
  ///   tile_renderer_.Render();
  /// (or not call it, but do some other staff - see modes implementation
  /// for more details)
  global_data_.UpdateCursorPos();

  cur_mode_->Render();

  //TODO; maybe GetIdByMousePos shouldn't take any parameters
  glm::dvec2 cursor_pos = global_data_.cursor_pos_;
  auto pressed_id = global_data_.picking_fbo_.GetIdByMousePos(cursor_pos);

//  int text_offset{GetUiDataTextOffset(UiVboDataTextId::kVision) * 4};
  int text_offset;
  if (glfwGetKey(gWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
    menu_.Render();
    text_offset = menu_.Hover(pressed_id);
  } else {
    text_offset = cur_mode_->Hover(pressed_id);
  }

  text_renderer_.RenderDescription(text_offset);

  picking_fbo_.Bind();
  cur_mode_->RenderPicking();
  if (glfwGetKey(gWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
    menu_.RenderPicking();
  }
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Interface::Init() {
  cur_mode_ = &terrain_;
  cur_mode_->BindCallbacks();
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void*>(&global_data_));
}
