#include "TerrainMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"

void TerrainModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<TerrainMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  //TODO; bvh
  if (!terrain->btn_bake_.AabbContains(cursor_pos) &&
      !terrain->btn_smooth_.AabbContains(cursor_pos) &&
      !terrain->slider_brush_size_.TrackAreaAabbContains(cursor_pos) &&
      !terrain->slider_brush_falloff_.TrackAreaAabbContains(cursor_pos)) {
    if (yoffset < 0.0f) {
      global_data->tile_renderer_.DownScale();
    } else {
      global_data->tile_renderer_.UpScale();
    }
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
      //TODO; bvh
      if (terrain->btn_bake_.AabbContains(cursor_pos)) {
        terrain->btn_bake_.Press(cursor_pos);
      } else if (terrain->btn_smooth_.AabbContains(cursor_pos)) {
        terrain->btn_smooth_.Press(cursor_pos);
      } else if (terrain->slider_brush_size_.TrackAreaAabbContains(cursor_pos)) {
        terrain->slider_brush_size_.UpdateSliderPos(cursor_pos);
        terrain->slider_brush_size_pressed_ = true;
      } else if (terrain->slider_brush_falloff_.TrackAreaAabbContains(cursor_pos)) {
        terrain->slider_brush_falloff_pressed_ = true;
        terrain->slider_brush_falloff_.UpdateSliderPos(cursor_pos);
      }
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (terrain->slider_brush_size_pressed_) {
      terrain->slider_brush_size_pressed_ = false;
    } else if (terrain->slider_brush_falloff_pressed_) {
      terrain->slider_brush_falloff_pressed_ = false;
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

void TerrainMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, TerrainModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, TerrainModeMouseButtonCallback);
//  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}

void TerrainMode::Render() {
  shared_resources_.tile_renderer.RenderPickingTerrain();
  shared_resources_.tile_renderer.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  glDisable(GL_DEPTH_TEST);

  btn_bake_.Render();
  btn_smooth_.Render();
//  slider_brush_size_.Render();
//  slider_brush_falloff_.Render();

  if (slider_brush_size_pressed_) {
    slider_brush_size_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data.cursor_pos_);
  }
  if (slider_brush_falloff_pressed_) {
    slider_brush_falloff_.UpdateSliderPos(
        shared_resources_.global_glfw_callback_data.cursor_pos_);
  }

  glEnable(GL_DEPTH_TEST);

}
