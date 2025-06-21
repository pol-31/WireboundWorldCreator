#include "UiTilesMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

void UiTilesMode::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer->DownScale();
  } else {
    global_data->tile_renderer->UpScale();
  }
}

void UiTilesMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto tiles = dynamic_cast<UiTilesMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Press(pressed_id);
    } else {
      tiles->ui_event_handler_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    tiles->ui_event_handler_.Release();
  }
}

void UiTilesMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

UiTilesMode::UiTilesMode(UiSharedResources& ui_shared_resources)
    : IUiMode(ui_shared_resources,
              UiStaticSprite{data::VboIdMain::kTilesUiTilesMode,
                             data::TextId::kNone}),
      btn_up_(data::VboIdMain::kTilesUp, data::TextId::kNone,
              [this]() {
                std::cout << "btn_up" << std::endl;
              }),
      btn_down_(data::VboIdMain::kTilesDown, data::TextId::kNone,
                [this]() {
                  std::cout << "btn_down" << std::endl;
                }),
      btn_left_(data::VboIdMain::kTilesLeft, data::TextId::kNone,
                [this]() {
                  std::cout << "btn_left" << std::endl;
                }),
      btn_right_(data::VboIdMain::kTilesRight, data::TextId::kNone,
                 [this]() {
                   std::cout << "btn_right" << std::endl;
                 }),
      ui_event_handler_({&btn_up_, &btn_down_, &btn_left_, &btn_right_}) {}

void UiTilesMode::Render() {


  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();
  sprite_mode_.Render();
  btn_up_.Render();
  btn_down_.Render();
  btn_left_.Render();
  btn_right_.Render();
}

void UiTilesMode::RenderPicking() {


  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  sprite_mode_.RenderPicking();
  btn_up_.RenderPicking();
  btn_down_.RenderPicking();
  btn_left_.RenderPicking();
  btn_right_.RenderPicking();
}

void UiTilesMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, ModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(gWindow));
  global_data->camera->SetMapCamera();
}

data::TextId UiTilesMode::Hover(std::uint32_t global_id) {
  return data::TextId::kNone;
}
