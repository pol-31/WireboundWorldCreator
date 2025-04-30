#include "Interface.h"

#ifndef NDEBUG
#include "../common/UiDebugger.h"
#endif // NDEBUG

Interface::Interface(const Paths& paths, CameraHandler& camera, Map& map)
    : tile_renderer_(paths, visibility_, map),
      text_renderer_(paths),
      picking_fbo_(),
      shared_resources_(paths, tile_renderer_.GetTile(), tile_renderer_, global_data_),
      global_data_(camera, tile_renderer_,
                   cur_mode_, menu_, picking_fbo_, ui_debugger_),
      ui_debugger_(paths, shared_resources_.vbo_ui_,
                   shared_resources_.vbo_ui_transform_,
                   global_data_.cursor_pos_tex_norm_),
      terrain_(shared_resources_),
      water_(shared_resources_, paths, text_renderer_),
      roads_(shared_resources_, text_renderer_),
      fences_(shared_resources_, text_renderer_),
      biomes_(shared_resources_),
      objects_(shared_resources_),
      placement_(shared_resources_, paths),
      tiles_(shared_resources_),
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

#ifndef NDEBUG
  bool debug_ui = glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
  if (debug_ui && !debug_ui_prev_) {
    // it's static, but... idk ui_debugger_.BindCallbacks() looks better
    debug::UiDebugger::BindCallbacks();
  } else if (!debug_ui && debug_ui_prev_) {
    cur_mode_->BindCallbacks();
    ui_debugger_.ApplyAndReset();
  }
  debug_ui_prev_ = debug_ui;
  if (debug_ui) {
    ui_debugger_.Update();
  }
#endif // NDEBUG

  cur_mode_->Render();
  auto pressed_id = global_data_.GetIdByMousePos();

//  int text_offset{GetUiDataTextOffset(UiVboDataTextId::kVision) * 4};
  int text_offset;
  if (glfwGetKey(gWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
    menu_.Render();
    text_offset = menu_.Hover(pressed_id);
    //TODO: shadowing for current mode (anyway we can't Hover())
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
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void*>(&global_data_));
  cur_mode_ = &terrain_;
  cur_mode_->BindCallbacks();
  ui_debugger_.UpdateForce();
}
