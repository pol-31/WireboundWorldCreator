#include "FencesMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"

void FencesModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto fences = dynamic_cast<FencesMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer_.DownScale();
  } else {
    global_data->tile_renderer_.UpScale();
  }
}

void FencesModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto fences = dynamic_cast<FencesMode*>(global_data->cur_mode_);
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
        if (pressed_id == fences->btn_bake_picket_.GetId()) {
          fences->BakePicket();
        } else if (pressed_id == fences->btn_bake_chain_linked_.GetId()) {
          fences->BakeChainLinked();
        } else if (pressed_id == fences->btn_bake_wooden_.GetId()) {
          fences->BakeWooden();
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
void FencesModeKeyCallback(
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

FencesMode::FencesMode(SharedResources& shared_resources)
    : IEditMode(shared_resources),
      btn_bake_picket_("bake as a picket fence",
                       GetUiData(UiVboDataMainId::kFencesPicket)),
      btn_bake_chain_linked_("bake as a chain linked fence",
                             GetUiData(UiVboDataMainId::kFencesChainLink)),
      btn_bake_wooden_("bake as a wooden fence",
                       GetUiData(UiVboDataMainId::kFencesWooden)),
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

void FencesMode::Render() {
  shared_resources_.tile_renderer.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader.Bind();

  btn_bake_picket_.Render();
  btn_bake_chain_linked_.Render();
  btn_bake_wooden_.Render();
  btn_create_.Render();
  btn_remove_.Render();
  slots_.Render(shared_resources_.static_sprite_shader);
}

void FencesMode::RenderPicking() {
  shared_resources_.tile_renderer.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader.Bind();

  btn_bake_picket_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_bake_chain_linked_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_bake_wooden_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_create_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  btn_remove_.RenderPicking(shared_resources_.static_sprite_picking_shader);
  slots_.RenderPicking(shared_resources_.static_sprite_picking_shader);
}

void FencesMode::Create(GLuint id) {
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

void FencesMode::Remove() {
  std::cout << "removed selected point set" << std::endl;
  //  points_data_.clear();
}

// auto insidePoints = GenHeightMap();
/// here water_height_map_ already contains either 0 or actual height
//    FloodFill(insidePoints);
//TODO: from points generate height map (!)
// wrt each triangle surface slope, we gen flow map (how - idk)

void FencesMode::BakePicket() {
  std::cout << "baked as a picket fence" << std::endl;
}
void FencesMode::BakeChainLinked() {
  std::cout << "baked as a chain linked fence" << std::endl;
}
void FencesMode::BakeWooden() {
  std::cout << "baked as a wooden fence" << std::endl;
}

void FencesMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, FencesModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, FencesModeMouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}
