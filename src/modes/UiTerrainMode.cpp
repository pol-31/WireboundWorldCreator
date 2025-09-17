#include "UiTerrainMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"

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
    terrain->slots_.Scroll(pressed_id, yoffset);
    global_data->windows->Scroll(pressed_id, yoffset);
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->cur_tile_.DownScale();
  } else {
    global_data->tile_renderer->cur_tile_.UpScale();
  }
  terrain->slots_.UpdateTransformUniform();
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
    std::cout << "Pressed id: " << pressed_id << std::endl;
    if (!global_data->windows->Press(pressed_id)) {
      if (!terrain->slots_.Press(pressed_id)) {
        terrain->ui_event_handler_.Press(pressed_id);
        std::cout << "shoudl press bake" << std::endl;
      }
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    terrain->ui_event_handler_.Release();
    terrain->slots_.Release();
    global_data->windows->Release();
  }
}

void UiTerrainMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  if (action == GLFW_PRESS) {
    bool shift_pressed = (mods == GLFW_MOD_SHIFT);
    bool ctrl_pressed = (mods == GLFW_MOD_CONTROL);
    if (key == GLFW_KEY_W) {
      global_data->camera->SetMoveForward();
    } else if (key == GLFW_KEY_A) {
      global_data->camera->SetMoveLeft();
    } else if (key == GLFW_KEY_S) {
      global_data->camera->SetMoveBackward();
    } else if (key == GLFW_KEY_D) {
      global_data->camera->SetMoveRight();
    } else if (key == GLFW_KEY_ESCAPE) {
      if (shift_pressed) {
        glfwSetWindowShouldClose(window, true);
      } else if (!global_data->windows->GetTopWindow() &&
                 global_data->windows->GetSize() == 0) {
        global_data->ui_renderer->AskForConfirmation(
            data::TextId::kConfirmationExit, []() {
              glfwSetWindowShouldClose(gWindow, true);
            });
      } else {
        global_data->windows->BtnEscape();
      }
    } else if (key == GLFW_KEY_ENTER) {
      global_data->windows->BtnEnter();
    } else if (key == GLFW_KEY_C) {
      terrain->toggle_flatten_.Press();
    } else if (key == GLFW_KEY_X) { // dir X
      terrain->ev_selected_dir_ = glm::vec3{1.0f, 0.0f, 0.0f};
    } else if (key == GLFW_KEY_Y) { // dir Y
      terrain->ev_selected_dir_ = glm::vec3{0.0f, 1.0f, 0.0f};
    } else if (key == GLFW_KEY_Z) { // dir Z
      terrain->ev_selected_dir_ = glm::vec3{0.0f, 0.0f, -1.0f};
    } else {
      glfwGetCursorPos(gWindow, &gEventMouseStartPosX, &gEventMouseStartPosY);
      if (key == GLFW_KEY_G) {
        global_data->event_queue.Append(terrain->ev_translate_selected_);
      } else if (key == GLFW_KEY_R) {
        global_data->event_queue.Append(terrain->ev_rotate_selected_);
      } else if (key == GLFW_KEY_T) {
        global_data->event_queue.Append(terrain->ev_scale_selected_);
      } else if (key == GLFW_KEY_V) {
        terrain->remembered_progress_ = 1.0f - terrain->slider_size_.GetProgressUnscaled();
        global_data->event_queue.Append(terrain->ev_scale_cursor_size_);
      } else if (key == GLFW_KEY_F) {
        terrain->remembered_progress_ = 1.0f - terrain->slider_falloff_.GetProgressUnscaled();
        global_data->event_queue.Append(terrain->ev_scale_cursor_falloff_);
      }
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      global_data->camera->SetMoveForward(0.0f);
    } else if (key == GLFW_KEY_A) {
      global_data->camera->SetMoveLeft(0.0f);
    } else if (key == GLFW_KEY_S) {
      global_data->camera->SetMoveBackward(0.0f);
    } else if (key == GLFW_KEY_D) {
      global_data->camera->SetMoveRight(0.0f);
    } else if (key == GLFW_KEY_G) {
      global_data->event_queue.Remove(terrain->ev_translate_selected_);
    } else if (key == GLFW_KEY_R) {
      global_data->event_queue.Remove(terrain->ev_rotate_selected_);
    } else if (key == GLFW_KEY_T) {
      global_data->event_queue.Remove(terrain->ev_scale_selected_);
    } else if (key == GLFW_KEY_V) {
      global_data->event_queue.Remove(terrain->ev_scale_cursor_size_);
    } else if (key == GLFW_KEY_F) {
      global_data->event_queue.Remove(terrain->ev_scale_cursor_falloff_);
    }
  }
}

UiTerrainMode::UiTerrainMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    Tile& cur_tile)
    : IUiMode(
          ui_shared_resources,
          window_queue,
          {data::VboIdMain::kTerrainTerrainMode}),
      btn_update_(data::VboIdMain::kTerrainUpdate,
          [this]() {
                    std::cout << "btn_update?" << std::endl;
                    ui_bake_.Bake(1000, 1000, 0.1f);
          }),
      sprite_flatten_(data::VboIdMain::kTerrainFlatten),
      toggle_flatten_(
          {data::VboIdMain::kTerrainFlattenOff,
           [this]() {
             std::cout << "toggle_flatten" << std::endl;
           }},
          {data::VboIdMain::kTerrainFlattenOn1},
          {data::VboIdMain::kTerrainFlattenOn2},
          {data::VboIdMain::kTerrainFlattenOn3}
          ),
      slider_size_(
          {data::VboIdMain::kTerrainSizeFill},
          {data::VboIdMain::kTerrainSizeBack},
          {data::VboIdMain::kTerrainSizeIcon},
          100.0f
          ),
      slider_falloff_(
          {data::VboIdMain::kTerrainFalloffFill},
          {data::VboIdMain::kTerrainFalloffBack},
          {data::VboIdMain::kTerrainFalloffIcon},
          1.0f
          ),
      btn_bake_(data::VboIdMain::kTerrainBake,
                [this]() {
                  this->ui_bake_.Show();
                }),
      ui_bake_(
          cur_tile,
          {data::VboIdMain::kTerrainBakeDesk},
          1.0f,
          {{data::VboIdMain::kTerrainBakeDeskPinBack, []() {}},
           {data::VboIdMain::kTerrainBakeDeskPinPoint}},
          ui_shared_resources_,
          window_queue,
          text_renderer,
          {data::VboIdMain::kTerrainBakeAccept},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeErosionStepLabel},
          data::TextId::kErosion},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeErosionStepInput},
           {data::VboIdMain::kSpareText2}},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeWeatheringStepLabel},
           data::TextId::kWeathering},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeWeatheringStepInput},
           {data::VboIdMain::kSpareText3}}),
      slots_(
          cur_tile, ui_shared_resources_, window_queue,
          text_renderer, slider_size_, slider_falloff_),
      ui_event_handler_({
          &btn_update_, &slider_size_,
          &slider_falloff_, &toggle_flatten_, &btn_bake_, &slots_}),
      ev_translate_selected_([this]() {
        glm::vec2 cursor_diff =
            this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_
            - glm::dvec2{gEventMouseStartPosX, gEventMouseStartPosY};
        this->slots_.TranslateSelected(
            this->ev_selected_dir_ * (cursor_diff.y / gWindowHeight));
      }),
      ev_rotate_selected_([this]() {
        glm::vec2 cursor_diff =
            this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_
            - glm::dvec2{gEventMouseStartPosX, gEventMouseStartPosY};
        this->slots_.RotateSelected(
            this->ev_selected_dir_ * (cursor_diff.y / gWindowHeight));
      }),
      ev_scale_selected_([this]() {
        glm::vec2 cursor_diff =
            this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_
            - glm::dvec2{gEventMouseStartPosX, gEventMouseStartPosY};
        this->slots_.ScaleSelected(
            this->ev_selected_dir_ * (cursor_diff.y / gWindowHeight));
      }),
      ev_scale_cursor_size_([this]() {
        slider_size_.SetMouseDiff(
            remembered_progress_,
            {gEventMouseStartPosX, gEventMouseStartPosY},
            this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_);
      }),
      ev_scale_cursor_falloff_([this]() {
        slider_falloff_.SetMouseDiff(
            remembered_progress_,
            {gEventMouseStartPosX, gEventMouseStartPosY},
            this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_);
      }) {}

void UiTerrainMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  glfwSetKeyCallback(gWindow, KeyCallback);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(gWindow));
  global_data->camera->SetInspectCamera();
}

int UiTerrainMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiTerrainMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kStrength) + 1;
}

void UiTerrainMode::Render() {
  //TODO: should we call it RenderUi() and RenderTerrain()?
  slots_.RenderGraph(); // should be first (terrain render before ui render)

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  sprite_flatten_.Render();
  sprite_mode_.Render();

  btn_update_.Render();
  btn_bake_.Render();

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

  ui_shared_resources_.tex_ui_.Bind();
  window_queue_.Render();
}



void UiTerrainMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_flatten_.RenderPicking();
  sprite_mode_.RenderPicking();

  btn_update_.RenderPicking();
  btn_bake_.RenderPicking();

  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();

  toggle_flatten_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  slots_.RenderPicking();

  window_queue_.RenderPicking();
}
