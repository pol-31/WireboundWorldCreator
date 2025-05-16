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
                   cur_mode_, menu_, picking_fbo_,
                   ui_debugger_, &shared_resources_),
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
            &tiles_, cur_mode_),
      ui_loading_(
          UiStaticSprite{vbos::VboIdMain::kLoading0, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading1, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading2, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading3, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading4, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading5, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading6, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading7, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading8, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading9, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kLoading10, vbos::VboIdText::kNone}),
      ui_pop_up_back_(
          UiStaticSprite{vbos::VboIdMain::kAcceptDeclineDesk, vbos::VboIdText::kNone},
          glm::vec2{0.0f}, glm::vec2{1.0f},
          glm::vec2{1.0f}, glm::vec2{1.0f},
          0.0f, 0.0f),
      ui_confirmation_(
          UiStaticSprite{vbos::VboIdMain::kAcceptDeclineDesk, vbos::VboIdText::kNone},
          shared_resources_,
          UiStaticSprite{vbos::VboIdMain::kObjectChugaister, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kObjectMavka, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kObjectVodyaniy, vbos::VboIdText::kNone},
          UiDynamicSprite{vbos::VboIdMain::kCross, vbos::VboIdText::kNone}) {
  Init();
}

void Interface::RunLoading() {
  shared_resources_.static_sprite_shader_.Bind();
  ui_loading_.Render((static_cast<int>(glfwGetTime()) % 10) / 10.0f);
}

void Interface::ShowConfirmationWindow() {
  shared_resources_.static_sprite_shader_.Bind();
  ui_confirmation_.Render(true);
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
    menu_.Render(true);
    text_offset = menu_.Hover(pressed_id);
    //TODO: shadowing for current mode (anyway we can't Hover())
  } else {
    menu_.Render(false); // TODO: подавився кісточкою
    text_offset = cur_mode_->Hover(pressed_id);
  }

  // stages:
  // - hidden(Hover() calls stage 2
  // - appear(dynamic objects, moving up, Picking only for desk)
  // - showed(until Hover(), Picking for all nested components)
  // - disappear(dynamic objects, moving up, Picking only for desk)

  RunLoading();
  ShowConfirmationWindow();


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
