#include "WaterMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"

void WaterModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<WaterMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer_.DownScale();
  } else {
    global_data->tile_renderer_.UpScale();
  }
}

void WaterModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<WaterMode*>(global_data->cur_mode_);
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
        if (pressed_id == water->btn_bake_lake_.GetId()) {
          water->BakeLake();
        } else if (pressed_id == water->btn_bake_river_.GetId()) {
          water->BakeRiver();
        } else if (pressed_id == water->btn_bake_waterfall_.GetId()) {
          water->BakeWaterfall();
        } else if (pressed_id == water->btn_create_.GetId()) {
          water->Create(pressed_id);
        } else if (pressed_id == water->btn_remove_.GetId()) {
          water->Remove();
        }
      }
    }
  }
}

//TODO: multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void WaterModeKeyCallback(
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

WaterMode::WaterMode(SharedResources& shared_resources)
    : IEditMode(shared_resources),
      btn_bake_lake_("bake as a lake",
                     GetUiData(UiVboDataMainId::kWaterLake)),
      btn_bake_river_("bake as a river",
                      GetUiData(UiVboDataMainId::kWaterRiver)),
      btn_bake_waterfall_("bake as a waterfall",
                          GetUiData(UiVboDataMainId::kWaterWaterfall)),
      btn_create_("add new point set",
                  GetUiData(UiVboDataMainId::kAddNew)),
      btn_remove_("remove selected point set",
                  GetUiData(UiVboDataMainId::kRemove)),
      slots_(points_data_test_,
             {"next slots", GetUiData(UiVboDataMainId::kUiSlotsNext)},
             {"prev slots", GetUiData(UiVboDataMainId::kUiSlotsPrev)},
             {"slot 1", GetUiData(UiVboDataMainId::kUiSlots1)},
             {"slot 2", GetUiData(UiVboDataMainId::kUiSlots2)},
             {"slot 3", GetUiData(UiVboDataMainId::kUiSlots3)},
             {"slot 4", GetUiData(UiVboDataMainId::kUiSlots4)},
             {"slot 5", GetUiData(UiVboDataMainId::kUiSlots5)},
             edit_mode_selected_sample_id_test_) {}

void WaterMode::Render() {
  shared_resources_.tile_renderer.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader.Bind();

  btn_bake_lake_.Render();
  btn_bake_river_.Render();
  btn_bake_waterfall_.Render();
  btn_create_.Render();
  btn_remove_.Render();
  slots_.Render(shared_resources_.static_sprite_shader);

//  RenderPoints();
}

void WaterMode::RenderPicking() {
  shared_resources_.tile_renderer.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader.Bind();

  btn_bake_lake_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_bake_river_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_bake_waterfall_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_create_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_remove_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slots_.RenderPicking(shared_resources_.static_sprite_picking_shader);
}

void WaterMode::Create(GLuint id) {
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

void WaterMode::Remove() {
  std::cout << "removed selected point set" << std::endl;
//  points_data_.clear();
}

// auto insidePoints = GenHeightMap();
/// here water_height_map_ already contains either 0 or actual height
//    FloodFill(insidePoints);
//TODO: from points generate height map (!)
// wrt each triangle surface slope, we gen flow map (how - idk)

void WaterMode::BakeLake() {
  std::cout << "bake as a lake" << std::endl;
}
void WaterMode::BakeRiver() {
  std::cout << "bake as a river" << std::endl;
}
void WaterMode::BakeWaterfall() {
  std::cout << "bake as a waterfall" << std::endl;
}

void WaterMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, WaterModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, WaterModeMouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}
