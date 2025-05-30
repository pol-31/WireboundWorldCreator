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
  auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1) &&
      fences->slots_.Scroll(pressed_id, yoffset)) {
    return;
  }
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
  global_data->camera_.ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu_.Press(pressed_id);
    } else {
      if (pressed_id < details::kIdOffsetWater) {
        std::cout << "Add point #" << pressed_id << std::endl;
        std::cout << "Coordinates are: " << (pressed_id & 1023)
                  << " and " << (pressed_id >> 10) << std::endl;
        //TODO: if water/other subtract maybe...
        fences->graphs_.Press(pressed_id);
      }
      fences->ui_event_handler_.Press(pressed_id);
      fences->slots_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu_.Release();
    } else {
      fences->ui_event_handler_.Release();
      fences->slots_.Release();
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

FencesMode::FencesMode(
    SharedResources& shared_resources, const TextRenderer& text_renderer)
    : IEditMode(shared_resources),
      btn_bake_picket_(vbos::VboIdMain::kFencesPicket, vbos::VboIdText::kBakeAsAPicketFence),
      btn_bake_chain_linked_(vbos::VboIdMain::kFencesChainLink,
                             vbos::VboIdText::kBakeAsAChainLinkedFence),
      btn_bake_wooden_(vbos::VboIdMain::kFencesWooden, vbos::VboIdText::kBakeAsAWoodenFence),
      btn_remove_(vbos::VboIdMain::kFencesRemove, vbos::VboIdText::kRemoveSelected),
      graphs_(shared_resources_),
      slots_(
          shared_resources_,
          {vbos::VboIdMain::kFencesSlotsHandler, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsSlider, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsBack, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsCreate, vbos::VboIdText::kNone,
           [this]() {
             this->graphs_.CreateGraph();
           }},
          vbos::VboIdMain::kFencesSlotsFlipSelectEdit_Back, vbos::VboIdText::kNone,
          vbos::VboIdMain::kFencesSlotsFlipPointEdge_Back, vbos::VboIdText::kNone,
          {vbos::VboIdMain::kFencesSlotsFlipSelectEdit, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsFlipPointEdge, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsSlot, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsRemove, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kFencesSlotsSelected, vbos::VboIdText::kNone},
          graphs_),
      ui_event_handler_({
          &btn_bake_picket_, &btn_bake_chain_linked_,
          &btn_bake_wooden_, &btn_remove_, &slots_}) {}

void FencesMode::Render() {
  shared_resources_.tile_renderer_.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  btn_bake_picket_.Render();
  btn_bake_chain_linked_.Render();
  btn_bake_wooden_.Render();
  btn_remove_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  auto mouse_pos
      = shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  slots_.Render(mouse_pos);

  graphs_.Render();
}

void FencesMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_bake_picket_.RenderPicking();
  btn_bake_chain_linked_.RenderPicking();
  btn_bake_wooden_.RenderPicking();
  btn_remove_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  slots_.RenderPicking();
}

void FencesMode::Remove() {
  graphs_.Remove();
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

int FencesMode::Hover(std::uint32_t global_id) {
  return -1;
}
