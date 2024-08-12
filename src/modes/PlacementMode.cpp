#include "PlacementMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"

void PlacementModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto placement = dynamic_cast<PlacementMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer_.DownScale();
  } else {
    global_data->tile_renderer_.UpScale();
  }
}

void PlacementModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto placement = dynamic_cast<PlacementMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      global_data->cursor_.SwitchMode(window);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu_.Press(pressed_id);
      } else {
        //TODO: bvh?
        if (pressed_id == placement->btn_place_trees_.GetId()) {
          placement->PlaceTrees();
        } else if (pressed_id == placement->btn_place_bushes_.GetId()) {
          placement->PlaceBushes();
        } else if (pressed_id == placement->btn_place_tall_grass_.GetId()) {
          placement->PlaceTallGrass();
        } else if (pressed_id == placement->btn_place_undergrowth_.GetId()) {
          placement->PlaceUndergrowth();
        } else if (pressed_id == placement->slider_color_.GetTrackId()) {
          placement->slider_color_pressed_ = true;
          placement->slider_color_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
        } else if (pressed_id == placement->slider_size_.GetTrackId()) {
          placement->slider_size_pressed_ = true;
          placement->slider_size_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
        } else if (pressed_id == placement->slider_falloff_.GetTrackId()) {
          placement->slider_falloff_pressed_ = true;
          placement->slider_falloff_.UpdateSliderPos(global_data->cursor_pos_tex_norm_);
        }
      }
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (placement->slider_color_pressed_) {
      placement->slider_color_pressed_ = false;
    } else if (placement->slider_size_pressed_) {
      placement->slider_size_pressed_ = false;
    } else if (placement->slider_falloff_pressed_) {
      placement->slider_falloff_pressed_ = false;
    }
  }
}

//TODO: multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void PlacementModeKeyCallback(
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

PlacementMode::PlacementMode(SharedResources& shared_resources)
    : IEditMode(shared_resources),
      btn_place_trees_(
          "place trees", GetUiData(UiVboDataMainId::kPlacementTrees)),
      btn_place_bushes_(
          "place bushes", GetUiData(UiVboDataMainId::kPlacementBushes)),
      btn_place_tall_grass_(
          "place tall grass", GetUiData(UiVboDataMainId::kPlacementTallGrass)),
      btn_place_undergrowth_(
          "place undergrowth", GetUiData(UiVboDataMainId::kPlacementUndergrowth)),
      slider_color_(
          {"min size", GetUiData(UiVboDataMainId::kPlacementSliderColorWhite)},
          {"max size", GetUiData(UiVboDataMainId::kPlacementSliderColorBlack)},
          {"size slider track", GetUiData(UiVboDataMainId::kPlacementSliderTrack)},
          {"size slider handle", GetUiData(UiVboDataMainId::kPlacementSliderHandler), color_factor_}),
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

void PlacementMode::Render() {
  shared_resources_.tile_renderer.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader.Bind();

  btn_place_trees_.Render();
  btn_place_bushes_.Render();
  btn_place_tall_grass_.Render();
  btn_place_undergrowth_.Render();

  if (slider_color_pressed_) {
    slider_color_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }
  if (slider_size_pressed_) {
    slider_size_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }
  if (slider_falloff_pressed_) {
    slider_falloff_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }

  slider_color_.Render(shared_resources_.static_sprite_shader,
                            shared_resources_.slider_handle_shader);
  slider_size_.Render(shared_resources_.static_sprite_shader,
                               shared_resources_.slider_handle_shader);
  slider_falloff_.Render(shared_resources_.static_sprite_shader,
                               shared_resources_.slider_handle_shader);
}

void PlacementMode::RenderPicking() {
  shared_resources_.tile_renderer.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader.Bind();

  btn_place_trees_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_place_bushes_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_place_tall_grass_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_place_undergrowth_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slider_color_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slider_size_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slider_falloff_.RenderPicking(shared_resources_.static_sprite_picking_shader);
}

void PlacementMode::PlaceTrees() {
  std::cout << "place trees" << std::endl;
}
void PlacementMode::PlaceBushes() {
  std::cout << "place bushes" << std::endl;
}
void PlacementMode::PlaceTallGrass() {
  std::cout << "place tall grass" << std::endl;
}
void PlacementMode::PlaceUndergrowth() {
  std::cout << "place undergrowth" << std::endl;
}

void PlacementMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, PlacementModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, PlacementModeMouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}
