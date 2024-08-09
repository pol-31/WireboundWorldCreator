#include "Interface.h"

Interface::Interface(const Paths& paths, Camera& camera, Cursor& cursor)
    : tile_renderer_(paths),
      global_data_(camera, cursor, tile_renderer_, cur_mode_),
      shared_resources_(paths, tile_renderer_, global_data_),
      terrain_(shared_resources_),
      water_(shared_resources_),
      roads_(shared_resources_),
      fences_(shared_resources_),
      biomes_(shared_resources_),
      objects_(shared_resources_),
      placement_(shared_resources_),
      tiles_(shared_resources_),
      menu_(cur_mode_) {
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

  if (global_data_.tab_pressed_) {
    menu_.Render();
    menu_.RenderHover(global_data_.cursor_pos_);
  } else {
//    cur_mode_->RenderHover(global_data_.cursor_pos_);
  }
}

void Interface::Init() {
  cur_mode_ = &terrain_;
  terrain_.BindCallbacks();
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void*>(&global_data_));
}
