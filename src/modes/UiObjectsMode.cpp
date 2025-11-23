#include "UiObjectsMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Camera.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"

UiObjectsMode::UiObjectsMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    Tile& cur_tile,
    ModelManager& mdl_manager)
    : IUiMode(
          ui_shared_resources,
          {data::VboIdMain::kObjectsObjectsMode}),
      ui_selection_(ui_shared_resources),
      mouse_transform_(ui_shared_resources),
      ui_slots_(cur_tile, ui_shared_resources_, window_queue, text_renderer),
      ui_edit_(ui_shared_resources_, window_queue),
      mdl_manager_(mdl_manager)
/*,
      ui_event_handler_({
          &btn_mavka_, &btn_vodyaniy_, &btn_chugaister_, &btn_pedestal_,
          &btn_campfire_, &btn_road_sign_, &btn_human_})*/ {
  ui_slots_.SetConfigWindow(&ui_edit_);
}

void UiObjectsMode::Setup() {
  objects::BindCallbacksDefault();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f); // to update camera vectors
  ui_selection_.SetIdBounds(details::kIdOffsetTerrain, details::kIdOffsetWater);
  ui_selection_.SetSelectionModeForce(SelectionMode::kRectangle);
  UpdateModelsList();
}

int UiObjectsMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiObjectsMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kStrength) + 1;
}

void UiObjectsMode::Render() {
  mdl_manager_.Render();
  ui_selection_.Render();
  ui_selection_.RenderSurfaceSelection(
      &ui_shared_resources_.global_glfw_callback_data_.tile_renderer
           ->cur_tile_.map_terrain_height);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  sp_mode_.Render();

  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.global_glfw_callback_data_.windows->Render();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->Update(1.0f); // const pos
}

void UiObjectsMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sp_mode_.RenderPicking();
  ui_slots_.RenderPicking();
  ui_shared_resources_.global_glfw_callback_data_.windows->RenderPicking();
  mdl_manager_.RenderPicking();
}

void UiObjectsMode::UpdateModelsList() {
  ui_slots_.Reset();
  const auto& models = mdl_manager_.GetLoadedModels();
  for (const auto& m : models) {
    BaseInstanceData data(std::to_string(m->vao), glm::vec4(1.0f), true, m->vao);
    ui_slots_.AddInstance(std::move(data));
  }
}

void UiObjectsMode::HandleSelection() {
  const auto& tex_selected = ui_selection_.GetSelectionMask();
  std::vector<uint8_t> selected_pixels(
      tex_selected.GetHeight() * tex_selected.GetWidth());
  tex_selected.Bind();
  glGetTexImage(
      GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, selected_pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  int selected_num = 0;
  for (auto& m : mdl_manager_.creatures_) {
    auto position = m.GetPosition();
    int x = static_cast<int>(position.x * 16.0f + 512.0f);
    int y = static_cast<int>(position.z * 16.0f + 512.0f);
    if (selected_pixels[y * 1024 + x] > 0) {
      m.Select();
      ++selected_num;
    } else {
      m.DeSelect();
    }
  }
  if (selected_num > 0) {
    anything_selected_ = true;
  } else {
    anything_selected_ = false;
  }
}

void UiObjectsMode::CancelTransform() {
  std::cout << "CancelTransform" << std::endl;
  mouse_transform_.CancelTransform();
  objects::BindCallbacksDefault();
  for (auto& m : mdl_manager_.creatures_) {
    if (m.IsSelected()) {
      auto obj_translate = m.GetPosition();
      auto obj_rotate = m.GetRotation();
      auto obj_scale = m.GetScale();
      m.SetPosition(obj_translate - prev_translate_);
      m.SetRotation(glm::inverse(prev_rotate_) * obj_rotate);
      m.SetScale(obj_scale / prev_scale_);
      m.UpdatePositionY(ui_shared_resources_);
    }
  }
}

void UiObjectsMode::ApplyTransform() {
  std::cout << "ApplyTransform" << std::endl;
  mouse_transform_.ApplyTransform();
  objects::BindCallbacksDefault();
  ResetTransform();
}

void UiObjectsMode::ResetTransform() {
  prev_translate_ = glm::vec3(0.0f);
  prev_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  prev_scale_ = glm::vec3(1.0f);
  cur_translate_ = glm::vec3(0.0f);
  cur_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  cur_scale_ = glm::vec3(1.0f);
}

void UiObjectsMode::UpdateTranslateForSelected() {
  glm::vec3 translate = -prev_translate_ + cur_translate_;
  for (auto& m : mdl_manager_.creatures_) {
    if (m.IsSelected()) {
      auto obj_translate = m.GetPosition();
      m.SetPosition(obj_translate + translate);
      m.UpdatePositionY(ui_shared_resources_);
    }
  }
  prev_translate_ = cur_translate_;
}

void UiObjectsMode::UpdateRotateForSelected() {
  glm::quat delta = cur_rotate_ * glm::inverse(prev_rotate_);
  for (auto& m : mdl_manager_.creatures_) {
    if (m.IsSelected()) {
      glm::quat obj_rot = m.GetRotation();
      obj_rot = delta * obj_rot;
      m.SetRotation(obj_rot);
    }
  }
  prev_rotate_ = cur_rotate_;
}

void UiObjectsMode::UpdateScaleForSelected() {
  glm::vec3 delta = cur_scale_ / prev_scale_;
  for (auto& m : mdl_manager_.creatures_) {
    if (m.IsSelected()) {
      glm::vec3 s = m.GetScale();
      m.SetScale(s * delta);
    }
  }
  prev_scale_ = cur_scale_;
}

void UiObjectsMode::SpawnObject(GLuint pressed_id) {
  glm::vec2 object_pos = {
      (pressed_id >> 10) / 16.0f - 32.0f,
      (pressed_id & 1023) / 16.0f - 32.0f
  };
  mdl_manager_.Spawn(ui_slots_.GetInstanceBaseData()->type_id, object_pos);
}

namespace objects {

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
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  if (objects->ui_selection_.ScrollSelection(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    global_data->windows->Scroll(pressed_id, yoffset);
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->cur_tile_.DownScale();
  } else {
    global_data->tile_renderer->cur_tile_.UpScale();
  }
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  //  global_data->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id =
          global_data->picking_fbo->GetIdByMousePos(cursor_pos);
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = global_data->windows->Press(pressed_id) ||
                        objects->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (objects->ui_slots_.GetSelectedSlotId() != -1 &&
          pressed_id < details::kIdOffsetWater) {
        objects->SpawnObject(pressed_id);
        return;
      }
      objects->ui_selection_.StartSelecting(
          global_data->cursor_pos_tex_norm_, mod_ctrl, mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (mod_shift) {
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_MmbShift);
      } else {
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_Mmb);
      }
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Mmb_MmbShift);
      glfwSetKeyCallback(gWindow, KeyCallback_Blocked);
    }
  } else { // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      global_data->windows->Release();
      objects->ui_selection_.Release();
      objects->ui_slots_.Release();
    }
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action != GLFW_PRESS) {
    return;
  }
  if (key == GLFW_KEY_ESCAPE) {
    if (mod_shift) {
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
  } else if (key == GLFW_KEY_1) {
    objects->ui_selection_.SetSelectionMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    objects->ui_selection_.SetSelectionMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    objects->ui_selection_.SetSelectionMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    objects->ui_selection_.SetSelectionMode(SelectionMode::kTweak);
  }
//  if (objects->ui_slots_.GetSelectedSlotId() == -1) {
//    return;
//  }
  if (key == GLFW_KEY_G) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
  } else if (key == GLFW_KEY_S) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
  } else if (key == GLFW_KEY_R) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
  }
}

/// --- transform ---

void BindCallbacksTransform() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallbackTransform);
  glfwSetKeyCallback(gWindow, KeyCallbackTransform);
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  objects->ResetTransform();
  objects->mouse_transform_.InitTransform(
      &objects->cur_translate_, &objects->cur_rotate_, &objects->cur_scale_);
}

void MouseButtonCallbackTransform(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    objects->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    //TODO: NextAxis();
  }
}

void KeyCallbackTransform(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  switch (key) {
    case GLFW_KEY_ESCAPE:
      return objects->CancelTransform();
    case GLFW_KEY_ENTER:
      return objects->ApplyTransform();
    case GLFW_KEY_G:
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
      return objects->CancelTransform();
    case GLFW_KEY_S:
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
      return objects->CancelTransform();
    case GLFW_KEY_R:
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
      return objects->CancelTransform();
  }
  if (mods & GLFW_MOD_SHIFT) {
    if (key == GLFW_KEY_X) {
      objects->mouse_transform_.SetAxis({0.0f, 1.0f, 1.0f});
    } else if (key == GLFW_KEY_Y) {
      objects->mouse_transform_.SetAxis({1.0f, 0.0f, 1.0f});
    } else if (key == GLFW_KEY_Z) {
      objects->mouse_transform_.SetAxis({1.0f, 1.0f, 0.0f});
    }
  } else {
    if (key == GLFW_KEY_X) {
      objects->mouse_transform_.SetAxis({1.0f, 0.0f, 0.0f});
    } else if (key == GLFW_KEY_Y) {
      objects->mouse_transform_.SetAxis({0.0f, 1.0f, 0.0f});
    } else if (key == GLFW_KEY_Z) {
      objects->mouse_transform_.SetAxis({0.0f, 0.0f, 1.0f});
    }
  }
}

void CursorPosCallback_G(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  objects->mouse_transform_.TranslateSelected(xpos, ypos);
  objects->UpdateTranslateForSelected();
}

void CursorPosCallback_R(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  objects->mouse_transform_.RotateSelected(xpos, ypos);
  objects->UpdateRotateForSelected();
}

void CursorPosCallback_S(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  objects->mouse_transform_.ScaleSelected(xpos, ypos);
  objects->UpdateScaleForSelected();
}

/// --- blocking ---


void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_PRESS) {
    BindCallbacksDefault();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    BindCallbacksDefault();
    objects->ui_selection_.StopSelecting(cursor_pos);
    objects->HandleSelection();
  }
}

void MouseButtonCallback_Mmb_MmbShift(
    GLFWwindow* window, int button, int action, int mods) {
  if (action == GLFW_PRESS ||
      (action == GLFW_RELEASE ||
       button == GLFW_MOUSE_BUTTON_MIDDLE)) {
    BindCallbacksDefault();
  }
}

void KeyCallback_Blocked(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  global_data->ui_renderer->Press(key, action);
  if (action == GLFW_RELEASE) {
    return;
  } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_ESCAPE) {
    BindCallbacksDefault();
  }
}

void CursorPosCallback_Mmb(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  Camera* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MoveRotateViewOrigin(xoffset, yoffset);
}

void CursorPosCallback_MmbShift(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  Camera* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MovePanView(xoffset, yoffset);
}

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*global_data->cur_mode);
  objects->ui_selection_.UpdateSelection(global_data->cursor_pos_tex_norm_);
}

} // namespace objects
