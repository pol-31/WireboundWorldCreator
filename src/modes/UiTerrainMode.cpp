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
      std::cout << "Release()" << std::endl;
    } else {
      terrain->ui_event_handler_.Release();
    }
  }
}

void UiTerrainMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

UiTerrainMode::UiTerrainMode(UiSharedResources& ui_shared_resources,
                             Tile& cur_tile)
    : IUiMode(
          ui_shared_resources,
          UiStaticSprite{data::VboIdMain::kTerrainTerrainMode,
                         data::TextId::kNotYet}),
      btn_update_(data::VboIdMain::kTerrainUpdate, data::TextId::kMenuPlacement,
          [this]() {
                    std::cout << "btn_update" << std::endl;
                    ui_terrain_generator_.Bake();
          }),
      sprite_flatten_(data::VboIdMain::kTerrainFlatten, data::TextId::kNotYet),
      toggle_flatten_(
          {data::VboIdMain::kTerrainFlattenOff, data::TextId::kNotYet,
           [this]() {
             std::cout << "toggle_flatten" << std::endl;
           }},
          {data::VboIdMain::kTerrainFlattenOn1, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainFlattenOn2, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainFlattenOn3, data::TextId::kNotYet}
          ),
      slider_size_(
          {data::VboIdMain::kTerrainSizeFill, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSizeBack, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSizeIcon, data::TextId::kNotYet}
          ),
      slider_falloff_(
          {data::VboIdMain::kTerrainFalloffFill, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainFalloffBack, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainFalloffIcon, data::TextId::kNotYet}
          ),
      btn_bake_({data::VboIdMain::kTerrainBake, data::TextId::kNotYet}),
      ui_bake_(
          ui_shared_resources_,
          {data::VboIdMain::kTerrainBakeDesk, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainBakeAccept, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainBakeHeightmap, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainBakeErosionStepLabel, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainBakeErosionStepInput, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainBakeWeatheringStepLabel, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainBakeWeatheringStepInput, data::TextId::kNotYet}),
      slots_(
          ui_shared_resources_,
          {data::VboIdMain::kTerrainSlotsHandler, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsSlider, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsBack, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsCreate, data::TextId::kNotYet,
           [this]() {
             this->slots_.CreateGraph();
             ui_shared_resources_.global_glfw_callback_data_.StartCharInput(
                 slots_.GetNamePtr(slots_.GetSize() - 1));
           }},
          data::VboIdMain::kTerrainSlotsFlipPointEdgeFace_Back, data::TextId::kNotYet,
          {data::VboIdMain::kTerrainSlotsFlipPointEdgeFace, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsName, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsConfig, data::TextId::kNotYet},
          {{data::VboIdMain::kTerrainVisibleOff, data::TextId::kNotYet},
              {data::VboIdMain::kTerrainVisibleOn1, data::TextId::kNotYet},
              {data::VboIdMain::kTerrainVisibleOn2, data::TextId::kNotYet},
              {data::VboIdMain::kTerrainVisibleOn3, data::TextId::kNotYet},
          },
          {data::VboIdMain::kTerrainSlotsSlot, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsSlotColor, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsRemove, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainSlotsSelected, data::TextId::kNotYet}),
      ui_event_handler_({
          &btn_update_, &slider_size_,
          &slider_falloff_, &toggle_flatten_, &btn_bake_, &slots_}),
      ui_terrain_generator_({data::VboIdMain::kMapLeaf, data::TextId::kNotYet},
                            1.0f, ui_shared_resources_, cur_tile) {}

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

  slider_size_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  slider_falloff_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);

  toggle_flatten_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  slider_size_.RenderIcon();
  slider_falloff_.RenderIcon();

  // Bake / Slots::Edit / NoiseEdit
//  if (render_slots_config_) {
//    config_.Render();
//  } else if (render_bake_config_) {
//    bake_config_.Render();
//  }
  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  slots_.Render(mouse_pos);
  ui_shared_resources_.tex_ui_.Bind();
  ui_bake_.Render();
}



void UiTerrainMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_flatten_.RenderPicking();
  sprite_mode_.RenderPicking();

  btn_update_.RenderPicking();

  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();

  toggle_flatten_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  slots_.RenderPicking();
  ui_bake_.RenderPicking();
}

data::TextId UiTerrainMode::Hover(std::uint32_t global_id) {
  return ui_event_handler_.Hover(global_id);
}
