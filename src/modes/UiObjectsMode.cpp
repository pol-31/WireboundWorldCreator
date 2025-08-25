#include "UiObjectsMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

void UiObjectsMode::ScrollCallback(
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

void UiObjectsMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Press(pressed_id);
    } else {
      objects->ui_event_handler_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->ui_event_handler_.Release();
  }
}

void UiObjectsMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

UiObjectsMode::UiObjectsMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue)
    : IUiMode(
          ui_shared_resources,
          window_queue,
          {data::VboIdMain::kObjectsObjectsMode}),
      btn_mavka_(data::VboIdMain::kObjectsMavka,
                [this]() {
                  this->BtnMavka();
                }),
      btn_vodyaniy_(data::VboIdMain::kObjectsVodyaniy,
               [this]() {
                 this->BtnVodyaniy();
               }),
      btn_chugaister_(data::VboIdMain::kObjectsChugaister,
                [this]() {
                  this->BtnChugaister();
                }),
      btn_pedestal_(data::VboIdMain::kObjectsPedestal,
          [this]() {
            this->BtnPedestal();
          }),
      btn_campfire_(data::VboIdMain::kObjectsCampfire,
          [this]() {
            this->BtnCampfire();
          }),
      btn_road_sign_(data::VboIdMain::kObjectsRoadSign,
                  [this]() {
                    this->BtnRoadSign();
                  }),
      btn_human_(data::VboIdMain::kObjectsHuman,
                  [this]() {
                    this->BtnHuman();
                  }),
      ui_event_handler_({
          &btn_mavka_, &btn_vodyaniy_, &btn_chugaister_, &btn_pedestal_,
          &btn_campfire_, &btn_road_sign_, &btn_human_}) {}

void UiObjectsMode::BtnMavka() {
  std::cout << "btn_mavka" << std::endl;
}

void UiObjectsMode::BtnVodyaniy() {
  std::cout << "btn_vodyaniy" << std::endl;
};

void UiObjectsMode::BtnChugaister() {
  std::cout << "btn_chugaister" << std::endl;
}

void UiObjectsMode::BtnPedestal() {
  std::cout << "btn_pedestal" << std::endl;
}

void UiObjectsMode::BtnCampfire() {
  std::cout << "btn_campfire" << std::endl;
}

void UiObjectsMode::BtnRoadSign() {
  std::cout << "btn_road_sign" << std::endl;
}

void UiObjectsMode::BtnHuman() {
  std::cout << "btn_human" << std::endl;
}

void UiObjectsMode::Render() {


  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();
  sprite_mode_.Render();
  btn_mavka_.Render();
  btn_vodyaniy_.Render();
  btn_chugaister_.Render();
  btn_pedestal_.Render();
  btn_campfire_.Render();
  btn_road_sign_.Render();
  btn_human_.Render();
}

void UiObjectsMode::RenderPicking() {


  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  sprite_mode_.RenderPicking();
  btn_mavka_.RenderPicking();
  btn_vodyaniy_.RenderPicking();
  btn_chugaister_.RenderPicking();
  btn_pedestal_.RenderPicking();
  btn_campfire_.RenderPicking();
  btn_road_sign_.RenderPicking();
  btn_human_.RenderPicking();
}

void UiObjectsMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}
