#include "TerrainMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"
#include "../common/ShadersBinding.h"

void TerrainModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer_.DownScale();
  } else {
    global_data->tile_renderer_.UpScale();
  }
}

void TerrainModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<TerrainMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera_.ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu_.Press(pressed_id);
      } else {
        std::cout << pressed_id << std::endl;
        if (pressed_id == terrain->btn_smooth_.GetId()) {
          terrain->SwitchSmooth();
        } else if (pressed_id == terrain->btn_bake_.GetId()) {
          terrain->Bake();
        } else if (pressed_id == terrain->slider_size_.GetTrackId()) {
          terrain->need_to_update_uniforms_ = true;
          terrain->slider_size_.Press();
        } else if (pressed_id == terrain->slider_falloff_.GetTrackId()) {
          terrain->need_to_update_uniforms_ = true;
          terrain->slider_falloff_.Press();
        }
      }
    }
  } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
    terrain->need_to_update_uniforms_ = false;
    terrain->slider_size_.Release();
    terrain->slider_falloff_.Release();
  }

}

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void TerrainModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
/*  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<TerrainMode*>(global_data->cur_mode_);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_LEFT_SHIFT) {
      global_data->camera_.SpeedUp();
    } else if (key == GLFW_KEY_BACKSPACE) {
      global_data->terrain_.ClearPoints();
    } else if (key == GLFW_KEY_ENTER) {
      global_data->terrain_.Bake();
    } else if (key == GLFW_KEY_TAB) {
      global_data->tab_pressed_ = true;
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_LEFT_SHIFT) {
      global_data->camera_.SlowDown();
    } else if (key == GLFW_KEY_TAB) {
      global_data->tab_pressed_ = false;
    }
  }*/
}

TerrainMode::TerrainMode(SharedResources& shared_resources)
    : IEditMode(shared_resources),
      btn_bake_(vbos::VboIdMain::kTerrainUpdate, vbos::VboIdText::kNone),
      btn_smooth_(vbos::VboIdMain::kTerrainSmooth, vbos::VboIdText::kNone),
      slider_size_(
          UiStaticSprite{vbos::VboIdMain::kTerrainSliderSizeFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kTerrainSliderSizeSlow, vbos::VboIdText::kNone}),
      slider_falloff_(
          UiStaticSprite{vbos::VboIdMain::kTerrainSliderFalloffFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kTerrainSliderFalloffSlow, vbos::VboIdText::kNone}) {}

void TerrainMode::Bake() {
  std::cout << "baked" << std::endl;
}

void TerrainMode::SwitchSmooth() {
  smooth_mode_ = !smooth_mode_;
  std::cout << "smooth mode enabled: " << std::boolalpha << smooth_mode_
            << std::noboolalpha << std::endl;
}

void TerrainMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, TerrainModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, TerrainModeMouseButtonCallback);
//  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(gWindow));
  global_data->camera_.SetInspectCamera();
}

void TerrainMode::Render() {
  shared_resources_.tile_renderer_.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  btn_bake_.Render();
  btn_smooth_.Render();

  slider_size_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);
  slider_falloff_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);

  if (need_to_update_uniforms_) {
    //TODO: here we need separate POINT-LINES shader and its mechanic
//    shader_draw_.Bind();
//    glUniform1f(shader::kPlacementRadius, slider_size_.GetProgress());
//    glUniform1f(shader::kPlacementFalloff, slider_falloff_.GetProgress());
//    glUseProgram(0);
//    need_to_update_uniforms_ = false;
  }

  double last_x_, last_y_;
  glfwGetCursorPos(gWindow, &last_x_, &last_y_);
//  std::cout << "LAST: " << last_x_ << ' ' << last_y_ << std::endl;
}

void TerrainMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_bake_.RenderPicking();
  btn_smooth_.RenderPicking();

  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();
}

int TerrainMode::Hover(std::uint32_t global_id) {
  if (global_id == btn_smooth_.GetId()) {
    return btn_smooth_.Hover();
  } else if (global_id == btn_bake_.GetId()) {
    return btn_bake_.Hover();
  } /*else if (global_id == terrain->slider_size_.GetTrackId()) {
    terrain->slider_size_pressed_ = true;
    terrain->slider_size_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
  } else if (global_id == terrain->slider_falloff_.GetTrackId()) {
    terrain->slider_falloff_pressed_ = true;
    terrain->slider_falloff_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
  }*/
  return -1;
}
