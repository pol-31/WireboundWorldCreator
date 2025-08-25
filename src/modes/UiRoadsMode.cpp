#include "UiRoadsMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

void UiRoadsMode::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer->cur_tile_.DownScale();
  } else {
    global_data->tile_renderer->cur_tile_.UpScale();
  }
}

void UiRoadsMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto fences = dynamic_cast<UiRoadsMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu->Press(pressed_id);
      } else {
        //TODO: bvh?
        if (pressed_id == fences->btn_bake_asphalt_.GetId()) {
          fences->BakeAsphalt();
        } else if (pressed_id == fences->btn_bake_gravel_.GetId()) {
          fences->BakeGravel();
        } else if (pressed_id == fences->btn_bake_soil_.GetId()) {
          fences->BakeSoil();
        }
      }
    }
  }
}

void UiRoadsMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  /*  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
      } else if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera->SpeedUp();
      } else if (key == GLFW_KEY_BACKSPACE) {
        global_data->terrain_.ClearPoints();
      } else if (key == GLFW_KEY_ENTER) {
        global_data->terrain_.Bake();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = true;
      }
    } else if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera->SlowDown();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = false;
      }
    }*/
}

UiRoadsMode::UiRoadsMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue)
    : IUiMode(
          ui_shared_resources,
          window_queue,
          {data::VboIdMain::kRoadsRoadsMode}),
      btn_bake_asphalt_(data::VboIdMain::kRoadsAsphalt),
      btn_bake_gravel_(data::VboIdMain::kRoadsGravel),
      btn_bake_soil_(data::VboIdMain::kRoadsSoil) {}

void UiRoadsMode::Render() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  sprite_mode_.Render();
  btn_bake_asphalt_.Render();
  btn_bake_gravel_.Render();
  btn_bake_soil_.Render();
}

void UiRoadsMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_mode_.RenderPicking();
  btn_bake_asphalt_.RenderPicking();
  btn_bake_gravel_.RenderPicking();
  btn_bake_soil_.RenderPicking();
}

void UiRoadsMode::Create(GLuint id) {
  std::cout << "created new point set" << std::endl;
  /*  if (points_data_.size() == 64) {
      std::cerr << "points overflow; rewriting last" << std::endl;
      points_data_.pop_back();
    }
    for (const auto& i : points_data_) {
      if (i == id) {
        return;
      }
    }
    std::cout << "Point id: " << id << std::endl;
    points_data_.push_back(id);*/
}

void UiRoadsMode::Remove() {
  std::cout << "removed selected point set" << std::endl;
  //  points_data_.clear();
}

// auto insidePoints = GenHeightMap();
/// here water_height_map_ already contains either 0 or actual height
//    FloodFill(insidePoints);
//TODO: from points generate height map (!)
// wrt each triangle surface slope, we gen flow map (how - idk)

void UiRoadsMode::BakeAsphalt() {
  std::cout << "baked as an asphalt road" << std::endl;
}
void UiRoadsMode::BakeGravel() {
  std::cout << "baked as a gravel road" << std::endl;
}
void UiRoadsMode::BakeSoil() {
  std::cout << "baked as a soil road" << std::endl;
}

void UiRoadsMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}
