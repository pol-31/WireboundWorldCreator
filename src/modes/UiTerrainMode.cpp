#include "UiTerrainMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"

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
           {data::VboIdMain::kTerrainBakeErosionStepInputText},
           {data::VboIdMain::kTerrainBakeErosionStepInputBack}},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeWeatheringStepLabel},
           data::TextId::kWeathering},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeWeatheringStepInputText},
           {data::VboIdMain::kTerrainBakeWeatheringStepInputBack}}),
      slots_(
          cur_tile, ui_shared_resources_, window_queue,
          text_renderer, slider_size_, slider_falloff_),
      ui_event_handler_({
          &btn_update_, &slider_size_,
          &slider_falloff_, &toggle_flatten_, &btn_bake_, &slots_})/*,
      ev_translate_selected_([this]() {}),
      ev_rotate_selected_([this]() {
        float cursor_diff =
            glm::distance(
                glm::vec2(this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_),
                glm::vec2{static_cast<float>(gEventMouseStartPosX),
                          static_cast<float>(gEventMouseStartPosY)});
        this->slots_.RotateSelected(
            this->ev_selected_dir_ * (cursor_diff / gWindowHeight));
      }),
      ev_scale_selected_([this]() {
        float cursor_diff =
            glm::distance(
                glm::vec2(this->ui_shared_resources_.global_glfw_callback_data_.cursor_pos_),
                glm::vec2{static_cast<float>(gEventMouseStartPosX),
                          static_cast<float>(gEventMouseStartPosY)});
        this->slots_.ScaleSelected(
            this->ev_selected_dir_ * (cursor_diff / gWindowHeight));
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
      })*/ {}

void UiTerrainMode::BindCallbacks() {
  terrain::BindCallbacksDefault();
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

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  sprite_flatten_.Render();
  sprite_mode_.Render();

  btn_update_.Render();
  btn_bake_.Render();

  slider_size_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  slider_falloff_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);

  toggle_flatten_.Render();

  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  slots_.Render(mouse_pos);
  ui_shared_resources_.tex_ui_.Bind();

  ui_shared_resources_.tex_ui_.Bind();
  window_queue_.Render();

  ui_shared_resources_.global_glfw_callback_data_.ui_renderer
      ->RenderWorldOrigin(cursor_pos_, glm::vec4(1.0f));
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

void UiTerrainMode::Cancel() {
  std::cout << "Cancel" << std::endl;
  mouse_process_ = false;
  glfwSetCursorPosCallback(gWindow, CallbackCursorPos);
}

void UiTerrainMode::Apply() {
  std::cout << "Apply" << std::endl;
  mouse_process_ = false;
  glfwSetCursorPosCallback(gWindow, CallbackCursorPos);
}

void UiTerrainMode::CancelTransform() {
  std::cout << "CancelTransform" << std::endl;
  slots_.CancelTransform();
  terrain::BindCallbacksDefault();
}

void UiTerrainMode::ApplyTransform() {
  std::cout << "ApplyTransform" << std::endl;
  slots_.ApplyTransform();
  terrain::BindCallbacksDefault();
}

void UiTerrainMode::SetOriginPosition(GLuint pressed_id) {
  if (pressed_id < details::kIdOffsetWater) {
    glm::uvec2 pos(pressed_id >> 10, pressed_id & 1023);
    cursor_pos_.x = pos.x / 16.0f - 32.0f;
    cursor_pos_.z = pos.y / 16.0f - 32.0f;
    cursor_pos_.y =
        ui_shared_resources_.global_glfw_callback_data_.tile_renderer
            ->cur_tile_.terrain_heights_[pressed_id];
  }
}

namespace terrain {

void BindCallbacksDefault() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

void ScrollCallback(
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
  //  terrain->slots_.UpdateTransformUniform();
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
//  global_data->camera->ProcessMouseKey(button, action, mods);

  if (terrain->mouse_process_) {
    if (action == GLFW_PRESS) {
      terrain->Cancel(); // we can press the same without release
    } else { // GLFW_RELEASE
      if (button == terrain->pressed_mouse_key_) {
        terrain->Apply();
      }
    }
  } else {
    if (action == GLFW_PRESS) {
      bool mod_shift = mods & GLFW_MOD_SHIFT;
      bool mod_ctrl = mods & GLFW_MOD_CONTROL;
      bool mod_alt = mods & GLFW_MOD_ALT;
      terrain->pressed_mouse_key_ = button;
      double xpos, ypos;
      glfwGetCursorPos(gWindow, &xpos, &ypos);
      lastX = xpos;
      lastY = ypos;
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
        std::cout << "Pressed id: " << pressed_id << std::endl;
        bool ui_handled = global_data->windows->Press(pressed_id) ||
                          terrain->slots_.Press(pressed_id) ||
                          terrain->ui_event_handler_.Press(pressed_id);
        if (ui_handled) {
          return;
        }
        if (mod_shift) {
          if (mod_ctrl) {
            glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbCtrlShift);
          } else {
            glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbShift);
          }
        } else if (mod_ctrl) {
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbCtrl);
        } else {
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
        }
        terrain->mouse_process_ = true;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (mod_shift) {
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_RmbShift);
          terrain->mouse_process_ = true;
          terrain->SetOriginPosition(pressed_id);
        }
      } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (mod_shift) {
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_MmbShift);
        } else {
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_Mmb);
        }
        terrain->mouse_process_ = true;
      }
    } else { // GLFW_RELEASE
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        terrain->slots_.Release();
        global_data->windows->Release();
      }
      // if GLFW_RELEASE... if not process... we haven't done anything...
    }
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);

  if (terrain->mouse_process_) {
    if (action != GLFW_RELEASE) { // GLFW_REPEAT, GLFW_PRESS
      if (key == GLFW_KEY_ENTER) {
        terrain->Apply();
      } else if (key == GLFW_KEY_ESCAPE) {
        terrain->Cancel();
      }
    }
  } else {
    if (action == GLFW_PRESS) {
      bool shift_pressed = (mods & GLFW_MOD_SHIFT);
      bool ctrl_pressed = (mods & GLFW_MOD_CONTROL);
      if (key == GLFW_KEY_ESCAPE) {
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
      } else if (key == GLFW_KEY_G) {
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
        BindCallbacksTransform();
      } else if (key == GLFW_KEY_S) {
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
        BindCallbacksTransform();
      } else if (key == GLFW_KEY_R) {
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
        BindCallbacksTransform();
      }
    }
  }
}

void CursorPosCallback_Mmb(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  CameraHandler* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MoveRotateView(xoffset, yoffset);
}

void CursorPosCallback_MmbShift(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  CameraHandler* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MovePanView(xoffset, yoffset);
}

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {}

void CursorPosCallback_LmbShift(
    GLFWwindow* window, double xpos, double ypos) {}

void CursorPosCallback_LmbCtrl(
    GLFWwindow* window, double xpos, double ypos) {}

void CursorPosCallback_LmbCtrlShift(
    GLFWwindow* window, double xpos, double ypos) {}

void CursorPosCallback_RmbShift(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  std::cout << "Pressed id: " << pressed_id << std::endl;
  terrain->SetOriginPosition(pressed_id);
}

/// ---

void BindCallbacksTransform() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, ScrollCallbackTransform);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallbackTransform);
  glfwSetKeyCallback(gWindow, KeyCallbackTransform);
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  terrain->transform_axis_ = glm::vec3(1.0f);
  terrain->slots_.InitTranslateStart();
  terrain->slots_.InitScaleStart();
  terrain->slots_.InitRotateStart();
}

void ScrollCallbackTransform(
    GLFWwindow* window, double xoffset, double yoffset) {
  /// no functionality
}

void MouseButtonCallbackTransform(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    terrain->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    terrain->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    //TODO: NextAxis();
  }
}

void KeyCallbackTransform(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);

  /// #1
  if (action != GLFW_PRESS) {
    return;
  }

  if (key == GLFW_KEY_ESCAPE) {
    return terrain->CancelTransform();
  } else if (key == GLFW_KEY_ENTER) {
    return terrain->ApplyTransform();
  } else if (key == GLFW_KEY_G) {
    terrain->CancelTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
    return;
  } else if (key == GLFW_KEY_S) {
    terrain->CancelTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
    return;
  } else if (key == GLFW_KEY_R) {
    terrain->CancelTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
    return;
  }

  if (mods & GLFW_MOD_SHIFT) {
    if (key == GLFW_KEY_X) {
      terrain->transform_axis_ = glm::vec3(0.0f, 1.0f, 1.0f);
    } else if (key == GLFW_KEY_Y) {
      terrain->transform_axis_ = glm::vec3(1.0f, 0.0f, 1.0f);
    } else if (key == GLFW_KEY_Z) {
      terrain->transform_axis_ = glm::vec3(1.0f, 1.0f, 0.0f);
    }
  } else {
    if (key == GLFW_KEY_X) {
      terrain->transform_axis_ = glm::vec3(1.0f, 0.0f, 0.0f);
    } else if (key == GLFW_KEY_Y) {
      terrain->transform_axis_ = glm::vec3(0.0f, 1.0f, 0.0f);
    } else if (key == GLFW_KEY_Z) {
      terrain->transform_axis_ = glm::vec3(0.0f, 0.0f, 1.0f);
    }
  }
}

void CursorPosCallback_G(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);

  float cursor_diff = glm::length(glm::vec2{xoffset, yoffset});
  auto camera_dir_right = global_data->camera->GetDirectionRight();
  auto camera_dir_top = global_data->camera->GetDirectionUp();
  auto translate =
      camera_dir_right * xoffset * cursor_diff / static_cast<float>(gWindowWidth)
      + camera_dir_top * yoffset * cursor_diff / static_cast<float>(gWindowHeight);
  terrain->slots_.TranslateSelected(translate * terrain->transform_axis_);
}

glm::vec3 GetEulerAngles(const glm::quat& q) {
  glm::vec3 euler;

  // roll (X axis)
  float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
  float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
  euler.x = std::atan2(sinr_cosp, cosr_cosp);

  // pitch (Y axis)
  float sinp = 2.0f * (q.w * q.y - q.z * q.x);
  if (std::abs(sinp) >= 1)
    euler.y = std::copysign(glm::half_pi<float>(), sinp); // clamp ±90°
  else
    euler.y = std::asin(sinp);

  // yaw (Z axis)
  float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
  float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
  euler.z = std::atan2(siny_cosp, cosy_cosp);

  return glm::degrees(euler); // у градусах
}

void PrintQuaternion(glm::quat quat) {
  glm::mat4 m = glm::mat4_cast(quat);

  // Extract angles manually (YXZ convention for example)
  float pitch = std::asin(-m[2][0]); // rotation around X
  float yaw   = std::atan2(m[1][0], m[0][0]); // rotation around Y
  float roll  = std::atan2(m[2][1], m[2][2]); // rotation around Z

  std::cout << "Pitch: " << glm::degrees(pitch)
            << " Yaw: "   << glm::degrees(yaw)
            << " Roll: "  << glm::degrees(roll) << '\n';
}

void CursorPosCallback_R(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);

  float map_scale = global_data->tile_renderer->cur_tile_.map_scale;

  glm::vec3 axis = terrain->transform_axis_;
  if (axis == glm::vec3(1.0f)) {
    auto camera_pos = global_data->camera->GetPosition();
    auto obj_pos = terrain->slots_.GetInstanceTransform() * map_scale;
    axis = glm::normalize(obj_pos - camera_pos);
  }
  terrain->slots_.RotateSelected(
      static_cast<float>(xpos), static_cast<float>(ypos), axis);
}

void CursorPosCallback_S(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);

  auto map_scale = global_data->tile_renderer->cur_tile_.map_scale;
  auto pos_3d = terrain->slots_.GetInstanceTransform();
  glm::vec4 object_centre_3d = glm::vec4(pos_3d.x, pos_3d.y, pos_3d.z, 1.0f);
  auto model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(map_scale));
  auto view = global_data->camera->GetViewMatrix();
  auto projection = global_data->camera->GetProjMatrix();
  auto mvp = projection * view * model;
  glm::vec4 clipPos = mvp * object_centre_3d;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  glm::vec2 delta((xpos / (gWindowWidth / 2.0f) - 1.0f) - ndc.x,
                  ndc.y - ((gWindowHeight - ypos) / (gWindowHeight / 2.0f) - 1.0f));
  float factor = static_cast<float>(gWindowWidth)
                 / static_cast<float>(gWindowHeight);
  glm::vec2 offset(delta.x, factor * delta.y);
  std::cout << offset.x << ' ' << offset.y << std::endl;

  float cursor_diff = glm::length(offset);
  terrain->slots_.ScaleSelected(cursor_diff, terrain->transform_axis_);
}

} // namespace terrain
