#include "TerrainMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"

void TerrainModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<TerrainMode*>(global_data->cur_mode_);
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
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      global_data->cursor_.SwitchMode(window);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu_.Press(pressed_id);
      } else {
        if (pressed_id == terrain->btn_smooth_.GetId()) {
          terrain->SwitchSmooth();
        } else if (pressed_id == terrain->btn_bake_.GetId()) {
          terrain->Bake();
        } else if (pressed_id == terrain->slider_size_.GetTrackId()) {
          terrain->slider_size_pressed_ = true;
          terrain->slider_size_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
        } else if (pressed_id == terrain->slider_falloff_.GetTrackId()) {
          terrain->slider_falloff_pressed_ = true;
          terrain->slider_falloff_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
        }
      }
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (terrain->slider_size_pressed_) {
      terrain->slider_size_pressed_ = false;
    } else if (terrain->slider_falloff_pressed_) {
      terrain->slider_falloff_pressed_ = false;
    }
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
      btn_bake_(
          "bake terrain (update all other components"
          " like water, roads, etc)",
          GetUiData(UiVboDataMainId::kTerrainUpdate)),
      btn_smooth_(
          "enable smooth mode (flatten the terrain)",
          GetUiData(UiVboDataMainId::kTerrainSmooth)),
      slider_size_(
          {"min size", GetUiData(UiVboDataMainId::kUiSliderSizeMin)},
          {"max size", GetUiData(UiVboDataMainId::kUiSliderSizeMax)},
          {"size slider track", GetUiData(UiVboDataMainId::kUiSliderSizeTrack)},
          {"size slider handle", GetUiData(UiVboDataMainId::kUiSliderSizeHandler), size_factor_}),
      slider_falloff_(
          {"min falloff", GetUiData(UiVboDataMainId::kUiSliderFalloffMin)},
          {"max falloff", GetUiData(UiVboDataMainId::kUiSliderFalloffMax)},
          {"falloff slider track", GetUiData(UiVboDataMainId::kUiSliderFalloffTrack)},
          {"falloff slider handle", GetUiData(UiVboDataMainId::kUiSliderFalloffHandler),
           falloff_factor_}) {}

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
}

void TerrainMode::Render() {
  shared_resources_.tile_renderer.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader.Bind();

  btn_bake_.Render();
  btn_smooth_.Render();

  if (slider_size_pressed_) {
    slider_size_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }
  if (slider_falloff_pressed_) {
    slider_falloff_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }

  slider_size_.Render(shared_resources_.static_sprite_shader,
                            shared_resources_.slider_handle_shader);
  slider_falloff_.Render(shared_resources_.static_sprite_shader,
                               shared_resources_.slider_handle_shader);
}

void TerrainMode::RenderPicking() {
  shared_resources_.tile_renderer.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader.Bind();

  btn_bake_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_smooth_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slider_size_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slider_falloff_.RenderPicking(shared_resources_.static_sprite_picking_shader);
}
