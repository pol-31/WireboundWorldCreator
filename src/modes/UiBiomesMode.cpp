#include "UiBiomesMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

void UiBiomesMode::ScrollCallback(
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

void UiBiomesMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Press(pressed_id);
    } else {
      biomes->ui_event_handler_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    biomes->ui_event_handler_.Release();
  }
}

void UiBiomesMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}


UiBiomesMode::UiBiomesMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue)
    : IUiMode(
          ui_shared_resources,
          window_queue,
          {data::VboIdMain::kBiomesBiomesMode, data::TextId::kNotYet}),
      btn_wind_(data::VboIdMain::kBiomesEditWindIcon, data::TextId::kNotYet,
                [this]() {
                  this->BtnWind();
                }),
      btn_sun_(data::VboIdMain::kBiomesEditSunIcon, data::TextId::kNotYet,
               [this]() {
                 this->BtnSun();
               }),
      btn_time_(data::VboIdMain::kBiomesEditTimeIcon, data::TextId::kNotYet,
                [this]() {
                  this->BtnTime();
                }),
      btn_precipitations_(data::VboIdMain::kBiomesEditPrecipitationIcon,
                          data::TextId::kNotYet,
          [this]() {
            this->BtnPrecipitations();
          }),
      btn_temperature_(data::VboIdMain::kBiomesEditTemperatureIcon, data::TextId::kNotYet,
          [this]() {
            this->BtnTemperature();
          }),
      btn_clouds_(data::VboIdMain::kBiomesEditCloudIcon, data::TextId::kNotYet,
                  [this]() {
                    this->BtnClouds();
                  }),
      ui_event_handler_({
          &btn_wind_, &btn_sun_, &btn_time_,
          &btn_precipitations_, &btn_temperature_, &btn_clouds_}) {}

void UiBiomesMode::BtnWind() {
  std::cout << "btn_wind" << std::endl;
}

void UiBiomesMode::BtnSun() {
  std::cout << "btn_sun" << std::endl;
}

void UiBiomesMode::BtnTime() {
  std::cout << "btn_time" << std::endl;
}

void UiBiomesMode::BtnPrecipitations() {
  std::cout << "btn_precipitations" << std::endl;
}

void UiBiomesMode::BtnTemperature() {
  std::cout << "btn_temperature" << std::endl;
}

void UiBiomesMode::BtnClouds() {
  std::cout << "btn_clouds" << std::endl;
}

void UiBiomesMode::Render() {


  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();
  sprite_mode_.Render();
  btn_wind_.Render();
  btn_sun_.Render();
  btn_time_.Render();
  btn_precipitations_.Render();
  btn_temperature_.Render();
  btn_clouds_.Render();
}

void UiBiomesMode::RenderPicking() {


  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  sprite_mode_.RenderPicking();
  btn_wind_.RenderPicking();
  btn_sun_.RenderPicking();
  btn_time_.RenderPicking();
  btn_precipitations_.RenderPicking();
  btn_temperature_.RenderPicking();
  btn_clouds_.RenderPicking();
}

void UiBiomesMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}

data::TextId UiBiomesMode::Hover(std::uint32_t global_id) {
  return data::TextId::kNone;
}
