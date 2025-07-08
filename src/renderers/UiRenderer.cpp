#include "UiRenderer.h"

#include "../io/Cameras.h"

UiRenderer::UiRenderer(
    const Paths& paths, GlobalGlfwCallbackData& global_glfw_data_,
    TileRenderer& tile_renderer)
    : ui_shared_resources_(paths, global_glfw_data_),
      ui_debugger_(paths, ui_shared_resources_.vbo_ui_,
                   ui_shared_resources_.vbo_ui_transform_,
                   global_glfw_data_.cursor_pos_tex_norm_),
      terrain_(ui_shared_resources_, tile_renderer.cur_tile_),
      water_(ui_shared_resources_, paths),
      roads_(ui_shared_resources_),
      fences_(ui_shared_resources_),
      biomes_(ui_shared_resources_),
      objects_(ui_shared_resources_),
      placement_(ui_shared_resources_, paths),
      tiles_(ui_shared_resources_),
      text_renderer_({data::VboIdMain::kFullScreen, data::TextId::kNotYet}, paths),
      menu_(ui_shared_resources_, text_renderer_, &terrain_,
            &water_, &roads_, &fences_, &placement_, &objects_, &biomes_,
            &tiles_, cur_mode_),
      ui_loading_(
          UiStaticSprite{data::VboIdMain::kLoading0, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading1, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading2, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading3, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading4, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading5, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading6, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading7, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading8, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading9, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kLoading10, data::TextId::kNotYet}),
      ui_confirmation_(
          UiDynamicSprite{data::VboIdMain::kConfirmationDesk, data::TextId::kNotYet},
          5.0f,
          ui_shared_resources_,
          UiStaticSprite{data::VboIdMain::kConfirmationAccept, data::TextId::kNotYet},
          UiStaticSprite{data::VboIdMain::kConfirmationDecline,
                         data::TextId::kNotYet}),
      compass_({data::VboIdMain::kCompass, data::TextId::kNotYet}) {
  Init();
}

void UiRenderer::Render(data::TextId description_id) {
#ifndef NDEBUG
  bool debug_ui = glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
  if (debug_ui) {
    // it's static, but... idk ui_debugger_.BindCallbacks() looks better
    debug::UiDebugger::BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Update();
  } else if (debug_ui_prev_) {
    cur_mode_->BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Reset();
  }
  debug_ui_prev_ = debug_ui;
  if (debug_ui) {
    debug::gCtrlMode = true;
  } else {
    debug::gCtrlMode = false;
  }
#endif // NDEBUG

  cur_mode_->Render();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  compass_.SetRotate(glm::radians(
      ui_shared_resources_.global_glfw_callback_data_.camera->GetYaw()));
  compass_.Render();

  render_menu_ = (glfwGetKey(gWindow, GLFW_KEY_TAB) == GLFW_PRESS);
  menu_.Render(render_menu_);

  if (render_menu_) {
//    text_renderer_.RenderMenuText(description_id);
  } else {
//    text_renderer_.RenderModeText(description_id);
  }
}

data::TextId UiRenderer::Hover(GLuint pressed_id) {
  data::TextId description_id;
  if (render_menu_) {
    description_id = menu_.Hover(pressed_id);
    if (description_id == data::TextId::kNone) {
      description_id = data::TextId::kMenu;
    }
  } else {
    description_id = cur_mode_->Hover(pressed_id);
    if (description_id != data::TextId::kNone) {
    }
  }
  return description_id;
}

void UiRenderer::RenderPicking(data::TextId description_id) {
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  compass_.RenderPicking();

  if (render_menu_) {
    menu_.RenderPicking();
    if (description_id != data::TextId::kNone) {
//      text_renderer_.RenderMenuTextPicking(description_id);
    }
  } else {
    cur_mode_->RenderPicking();
    if (description_id != data::TextId::kNone) {
//      text_renderer_.RenderModeTextPicking(description_id);
    }
  }
}

void UiRenderer::Parse() {
  menu_.Parse();
  terrain_.Parse();
  water_.Parse();
  fences_.Parse();
  roads_.Parse();
  biomes_.Parse();
  objects_.Parse();
  placement_.Parse();
  tiles_.Parse();
}

void UiRenderer::Serialize() {
  menu_.Serialize();
  terrain_.Serialize();
  water_.Serialize();
  fences_.Serialize();
  roads_.Serialize();
  biomes_.Serialize();
  objects_.Serialize();
  placement_.Serialize();
  tiles_.Serialize();
}

void UiRenderer::Init() {
  text_renderer_.PrerenderMenuText(0, 24);
  ui_debugger_.ForceUpdate();
}

void UiRenderer::SetupGlobalData() {
  ui_shared_resources_.global_glfw_callback_data_.cur_mode = &cur_mode_;
  ui_shared_resources_.global_glfw_callback_data_.menu = &menu_;
  ui_shared_resources_.global_glfw_callback_data_.ui_debugger = &ui_debugger_;
  ui_shared_resources_.global_glfw_callback_data_.ui_shared_resources = &ui_shared_resources_;
  ui_shared_resources_.global_glfw_callback_data_.text_renderer = &text_renderer_;
}
