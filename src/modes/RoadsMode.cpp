#include "RoadsMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"

void RoadsModeScrollCallback(
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

void RoadsModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto fences = dynamic_cast<RoadsMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera_.ProcessMouseKey(button, action, mods);
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu_.Press(pressed_id);
      } else {
        //TODO: bvh?
        if (pressed_id == fences->btn_bake_asphalt_.GetId()) {
          fences->BakeAsphalt();
        } else if (pressed_id == fences->btn_bake_gravel_.GetId()) {
          fences->BakeGravel();
        } else if (pressed_id == fences->btn_bake_soil_.GetId()) {
          fences->BakeSoil();
        } else if (pressed_id == fences->btn_create_.GetId()) {
          fences->Create(pressed_id);
        } else if (pressed_id == fences->btn_remove_.GetId()) {
          fences->Remove();
        }
      }
    }
  }
}

//TODO: multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void RoadsModeKeyCallback(
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

RoadsMode::RoadsMode(
    SharedResources& shared_resources, const TextRenderer& text_renderer)
    : IEditMode(shared_resources),
      btn_bake_asphalt_(vbos::VboIdMain::kRoadsAsphalt, vbos::VboIdText::kBakeAsAnAsphaltRoad),
      btn_bake_gravel_(vbos::VboIdMain::kRoadsGravel, vbos::VboIdText::kBakeAsAGravelRoad),
      btn_bake_soil_(vbos::VboIdMain::kRoadsSoil, vbos::VboIdText::kBakeAsASoilRoad),
      btn_create_(vbos::VboIdMain::kRoadsAdd, vbos::VboIdText::kAddNew),
      btn_remove_(vbos::VboIdMain::kRoadsRemove, vbos::VboIdText::kRemoveSelected),
      slots_(temp_size_,
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kNextSlot},
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kPreviousSlot},
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kNone},
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kNone},
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kNone},
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kNone},
             UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kNone},
             edit_mode_selected_sample_id_test_, text_renderer) {}

void RoadsMode::Render() {
  shared_resources_.tile_renderer_.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  btn_bake_asphalt_.Render();
  btn_bake_gravel_.Render();
  btn_bake_soil_.Render();
  btn_create_.Render();
  btn_remove_.Render();
  slots_.Render();
}

void RoadsMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_bake_asphalt_.RenderPicking();
  btn_bake_gravel_.RenderPicking();
  btn_bake_soil_.RenderPicking();
  btn_create_.RenderPicking();
  btn_remove_.RenderPicking();
  slots_.RenderPicking();
}

void RoadsMode::Create(GLuint id) {
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

void RoadsMode::Remove() {
  std::cout << "removed selected point set" << std::endl;
  //  points_data_.clear();
}

// auto insidePoints = GenHeightMap();
/// here water_height_map_ already contains either 0 or actual height
//    FloodFill(insidePoints);
//TODO: from points generate height map (!)
// wrt each triangle surface slope, we gen flow map (how - idk)

void RoadsMode::BakeAsphalt() {
  std::cout << "baked as an asphalt road" << std::endl;
}
void RoadsMode::BakeGravel() {
  std::cout << "baked as a gravel road" << std::endl;
}
void RoadsMode::BakeSoil() {
  std::cout << "baked as a soil road" << std::endl;
}

void RoadsMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, RoadsModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, RoadsModeMouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}

int RoadsMode::Hover(std::uint32_t global_id) {
  return -1;
}
