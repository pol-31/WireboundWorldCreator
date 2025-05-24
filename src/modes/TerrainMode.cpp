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

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu_.Press(pressed_id);
    } else {
      terrain->ui_event_handler_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    terrain->ui_event_handler_.Release();
  }
}

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void TerrainModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

TerrainMode::TerrainMode(SharedResources& shared_resources)
    : IEditMode(shared_resources),
      sprite_flatten_(
          vbos::VboIdMain::kTerrainFlatten, vbos::VboIdText::kNone),
      sprite_terrain_mode_(
          vbos::VboIdMain::kTerrainTerrainMode, vbos::VboIdText::kNone),
      btn_update_(
          vbos::VboIdMain::kTerrainUpdate, vbos::VboIdText::kNone,
          [this]() {
            this->BtnUpdate();
          }),
      btn_regenerate_(
          vbos::VboIdMain::kTerrainRegenerate, vbos::VboIdText::kNone,
          [this]() {
            this->BtnRegenerate();
          }),
      slider_size_(
          UiStaticSprite{vbos::VboIdMain::kTerrainSizeFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kTerrainSizeBack, vbos::VboIdText::kNone},
          UiDynamicSprite{vbos::VboIdMain::kTerrainSizeIcon, vbos::VboIdText::kNone}
          ),
      slider_falloff_(
          UiStaticSprite{vbos::VboIdMain::kTerrainFalloffFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kTerrainFalloffBack, vbos::VboIdText::kNone},
          UiDynamicSprite{vbos::VboIdMain::kTerrainFalloffIcon, vbos::VboIdText::kNone}
          ),
      toggle_flatten_(
          UiStaticSprite{vbos::VboIdMain::kTerrainFlattenOff, vbos::VboIdText::kNone,
                         [this]() {
                           this->ToggleFlatten();
                         }},
          UiStaticSprite{vbos::VboIdMain::kTerrainFlattenOn1, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kTerrainFlattenOn2, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kTerrainFlattenOn3, vbos::VboIdText::kNone}
          ),
      ui_event_handler_({
          &btn_update_, &btn_regenerate_, &slider_size_,
          &slider_falloff_, &toggle_flatten_}) {}


void TerrainMode::BtnUpdate() {
  std::cout << "btn_update" << std::endl;
}

void TerrainMode::BtnRegenerate() {
  std::cout << "btn_regenerate" << std::endl;
}

void TerrainMode::ToggleFlatten() {
  std::cout << "toggle_flatten" << std::endl;
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

  sprite_flatten_.Render();
  sprite_terrain_mode_.Render();

  btn_update_.Render();
  btn_regenerate_.Render();

  slider_size_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  slider_falloff_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);

  toggle_flatten_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  slider_size_.RenderIcon();
  slider_falloff_.RenderIcon();
}

void TerrainMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_flatten_.RenderPicking();
  sprite_terrain_mode_.RenderPicking();

  btn_update_.RenderPicking();
  btn_regenerate_.RenderPicking();

  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();

  toggle_flatten_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
}

int TerrainMode::Hover(std::uint32_t global_id) {
  // NOT IMPLEMENTED
  return -1;
}
