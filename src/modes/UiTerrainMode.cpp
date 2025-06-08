#include "UiTerrainMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

void UiTerrainMode::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    terrain->slider_falloff_.Scroll(pressed_id, yoffset);
    terrain->slider_size_.Scroll(pressed_id, yoffset);
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->DownScale();
  } else {
    global_data->tile_renderer->UpScale();
  }
}

void UiTerrainMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Press(pressed_id);
    } else {
      terrain->ui_event_handler_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Release();
    } else {
      terrain->ui_event_handler_.Release();
    }
  }
}

void UiTerrainMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

UiTerrainMode::UiTerrainMode(UiSharedResources& ui_shared_resources)
    : IUiMode(ui_shared_resources,
              UiStaticSprite{data::VboIdMain::kTerrainUiTerrainMode,
                             data::TextId::kNone}),
      sprite_flatten_(data::VboIdMain::kTerrainFlatten, data::TextId::kNone),
      btn_update_(data::VboIdMain::kTerrainUpdate, data::TextId::kMenuPlacement,
          [this]() {
                    std::cout << "btn_update" << std::endl;
          }),
      btn_regenerate_(data::VboIdMain::kTerrainRegenerate, data::TextId::kNone,
          [this]() {
                        std::cout << "btn_regenerate" << std::endl;
                        struct TerrainParams {
                          struct Noise {
                            float resolution;           // slider
                            glm::vec2 tex_offset; // slider
                            Texture texture;
                          };

                        };
                        TerrainParams params;
          }),
      slider_size_(
          UiStaticSprite{data::VboIdMain::kTerrainSizeFill, data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kTerrainSizeBack, data::TextId::kNone},
          UiDynamicSprite{data::VboIdMain::kTerrainSizeIcon, data::TextId::kNone}
          ),
      slider_falloff_(
          UiStaticSprite{data::VboIdMain::kTerrainFalloffFill, data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kTerrainFalloffBack, data::TextId::kNone},
          UiDynamicSprite{data::VboIdMain::kTerrainFalloffIcon,
                          data::TextId::kNone}
          ),
      toggle_flatten_(
          UiStaticSprite{data::VboIdMain::kTerrainFlattenOff, data::TextId::kNone,
                         [this]() {
                           std::cout << "toggle_flatten" << std::endl;
                         }},
          UiStaticSprite{data::VboIdMain::kTerrainFlattenOn1, data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kTerrainFlattenOn2, data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kTerrainFlattenOn3, data::TextId::kNone}
          ),
      ui_event_handler_({
          &btn_update_, &btn_regenerate_, &slider_size_,
          &slider_falloff_, &toggle_flatten_}) {}

void UiTerrainMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
//  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(gWindow));
  global_data->camera->SetInspectCamera();
}

glm::uvec2 UiTerrainMode::GetPrerenderTextIds() const noexcept {
  return {0, 24};
}

void UiTerrainMode::Render() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  sprite_flatten_.Render();
  sprite_mode_.Render();

  btn_update_.Render();
  btn_regenerate_.Render();

  slider_size_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  slider_falloff_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);

  toggle_flatten_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  slider_size_.RenderIcon();
  slider_falloff_.RenderIcon();

}

void UiTerrainMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_flatten_.RenderPicking();
  sprite_mode_.RenderPicking();

  btn_update_.RenderPicking();
  btn_regenerate_.RenderPicking();

  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();

  toggle_flatten_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
}

int UiTerrainMode::Hover(std::uint32_t global_id) {
  return ui_event_handler_.Hover(global_id);
}
