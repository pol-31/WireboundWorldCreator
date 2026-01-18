#include "UiTilesMode.h"

#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"

void UiTilesMode::ScrollCallback(GLFWwindow* window, double xoffset,
                                 double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void UiTilesMode::MouseButtonCallback(GLFWwindow* window, int button,
                                      int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto tiles = dynamic_cast<UiTilesMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) != GLFW_PRESS) {
      tiles->ui_event_handler_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    tiles->ui_event_handler_.Release();
  }
}

void UiTilesMode::KeyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {}

UiTilesMode::UiTilesMode(UiSharedResources& ui_shared_resources,
                         WindowQueue& window_queue)
    : IUiMode(ui_shared_resources, {data::VboIdMain::kTilesTilesMode}),
      btn_up_(data::VboIdMain::kTilesUp,
              [this]() { std::cout << "btn_up" << std::endl; }),
      btn_down_(data::VboIdMain::kTilesDown,
                [this]() { std::cout << "btn_down" << std::endl; }),
      btn_left_(data::VboIdMain::kTilesLeft,
                [this]() { std::cout << "btn_left" << std::endl; }),
      btn_right_(data::VboIdMain::kTilesRight,
                 [this]() { std::cout << "btn_right" << std::endl; }),
      ui_event_handler_({&btn_up_, &btn_down_, &btn_left_, &btn_right_}) {}

void UiTilesMode::Render() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.shader_sp_.Bind();
  sp_mode_.Render();
  btn_up_.Render();
  btn_down_.Render();
  btn_left_.Render();
  btn_right_.Render();
}

void UiTilesMode::RenderWorld() {
  ui_shared_resources_.glfw_context_.tile_renderer->Render();
}

void UiTilesMode::RenderPickingWorld() {
  ui_shared_resources_.glfw_context_.tile_renderer->RenderPicking();
}

void UiTilesMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  btn_up_.RenderPicking();
  btn_down_.RenderPicking();
  btn_left_.RenderPicking();
  btn_right_.RenderPicking();
}

void UiTilesMode::Setup() {}
