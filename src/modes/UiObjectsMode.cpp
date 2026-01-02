#include "UiObjectsMode.h"

#include "../common/Callbacks.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer.h"

UiObjectsMode::UiObjectsMode(UiSharedResources& ui_shared_resources,
                             UiSlots& ui_slots, WindowQueue& window_queue,
                             TextRenderer& text_renderer,
                             ModelManager& mdl_manager)
    : IUiMode(ui_shared_resources, {data::VboIdMain::kObjectsObjectsMode}),
      ui_selection_(ui_shared_resources),
      mouse_transform_(ui_shared_resources),
      ui_slots_(ui_slots),
      ui_edit_(ui_shared_resources, window_queue, text_renderer, models_,
               ui_slots_.GetSelectedIdRef(), mdl_manager),
      mdl_manager_(mdl_manager) {}

void UiObjectsMode::Setup() {
  ui_slots_.Setup(&models_, &ui_edit_);
  BindDefaultCallbacks();
  auto camera = ui_shared_resources_.gltf_context_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f);  // to update camera vectors
  ui_selection_.SetIdBounds(details::kIdOffsetTerrain, details::kIdOffsetWater);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
  UpdateModelsList();
}

void UiObjectsMode::BindDefaultCallbacks() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, objects::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, objects::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, objects::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

int UiObjectsMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiObjectsMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kLayer3) + 1;
}

void UiObjectsMode::RenderWorld() {
  ui_shared_resources_.gltf_context_.tile_renderer->Render();
}

void UiObjectsMode::RenderPickingWorld() {
  ui_shared_resources_.gltf_context_.tile_renderer->RenderPicking();
}

void UiObjectsMode::Render() {
  mdl_manager_.Render();
  ui_selection_.Render();
  ui_selection_.RenderOnSurface(
      &ui_shared_resources_.gltf_context_.tile_renderer->cur_tile_
           .map_terrain_height);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();
  sp_mode_.Render();

  auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.gltf_context_.windows->Render();
  auto camera = ui_shared_resources_.gltf_context_.camera;
  camera->Update(1.0f);  // const pos
}

void UiObjectsMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  ui_slots_.RenderPicking();
  ui_shared_resources_.gltf_context_.windows->RenderPicking();
  mdl_manager_.RenderPicking();
}

void UiObjectsMode::UpdateModelsList() {
  ui_slots_.Reset();
  const auto& models = mdl_manager_.GetLoadedModels();
  for (const auto& m : models) {
    BaseInstanceData data(std::to_string(m->vao), glm::vec4(1.0f), true,
                          m->vao);
    ui_slots_.AddInstance(std::move(data));
  }
}

void UiObjectsMode::HandleSelection() {
  const auto& tex_selected = ui_selection_.GetMask();
  std::vector<uint8_t> selected_pixels(tex_selected.GetHeight() *
                                       tex_selected.GetWidth());
  tex_selected.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                selected_pixels.data());
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
  std::cout << "selected_num " << selected_num << std::endl;
  if (selected_num > 0) {
    anything_selected_ = true;
  } else {
    anything_selected_ = false;
  }
}

void UiObjectsMode::CancelTransform() {
  std::cout << "CancelTransform" << std::endl;
  mouse_transform_.CancelTransform();
  BindDefaultCallbacks();
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
  BindDefaultCallbacks();
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
  glm::vec2 object_pos = {(pressed_id >> 10) / 16.0f - 32.0f,
                          (pressed_id & 1023) / 16.0f - 32.0f};
  mdl_manager_.Spawn(ui_slots_.GetInstanceBaseData()->type_id, object_pos);
}

namespace objects {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  if (objects->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    glfw_context->windows->Scroll(pressed_id, yoffset);
    return;
  }
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        objects->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (objects->ui_slots_.GetSelectedSlotId() != -1 &&
          pressed_id < details::kIdOffsetWater) {
        objects->SpawnObject(pressed_id);
        return;
      }
      objects->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_, mod_ctrl,
                                   mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (mod_shift) {
        glfwSetCursorPosCallback(gWindow,
                                 callbacks::CursorPosCallback_MmbShift);
      } else {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_Mmb);
      }
      glfwSetMouseButtonCallback(gWindow,
                                 callbacks::MouseButtonCallback_Mmb_MmbShift);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    }
  } else {  // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      glfw_context->windows->Release();
      objects->ui_slots_.Release();
    }
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action != GLFW_PRESS) {
    return;
  }
  if (key == GLFW_KEY_ESCAPE) {
    if (mod_shift) {
      glfwSetWindowShouldClose(window, true);
    } else if (!glfw_context->windows->GetTopWindow() &&
               glfw_context->windows->GetSize() == 0) {
      glfw_context->ui_renderer->AskForConfirmation(
          data::TextId::kConfirmationExit,
          []() { glfwSetWindowShouldClose(gWindow, true); });
    } else {
      glfw_context->windows->BtnEscape();
    }
  } else if (key == GLFW_KEY_ENTER) {
    glfw_context->windows->BtnEnter();
  } else if (key == GLFW_KEY_1) {
    objects->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    objects->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    objects->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    objects->ui_selection_.SetMode(SelectionMode::kTweak);
  }
  if (!objects->anything_selected_) {
    return;
  }
  if (key == GLFW_KEY_G) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
  } else if (key == GLFW_KEY_S) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
  } else if (key == GLFW_KEY_R) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
  } else if (key == GLFW_KEY_C) {
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    objects->ui_cloning_.Start(glfw_context->cursor_pos_tex_norm_);
    glfwSetScrollCallback(gWindow, nullptr);
    glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_C);
    glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_C);
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
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->ResetTransform();
  objects->mouse_transform_.InitTransform(
      &objects->cur_translate_, &objects->cur_rotate_, &objects->cur_scale_);
}

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    objects->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    // TODO: NextAxis();
  }
}

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);
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

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->mouse_transform_.TranslateSelected(xpos, ypos);
  objects->UpdateTranslateForSelected();
}

void CursorPosCallback_R(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->mouse_transform_.RotateSelected(xpos, ypos);
  objects->UpdateRotateForSelected();
}

void CursorPosCallback_S(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->mouse_transform_.ScaleSelected(xpos, ypos);
  objects->UpdateScaleForSelected();
}

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->ui_selection_.Update(glfw_context->cursor_pos_tex_norm_);
}

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  //  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  if (action == GLFW_PRESS) {
    objects->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->BindDefaultCallbacks();
    objects->ui_selection_.Stop(glfw_context->cursor_pos_tex_norm_);
    objects->HandleSelection();
  }
}

/// cloning

void CursorPosCallback_C(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->ui_cloning_.Update(glfw_context->cursor_pos_tex_norm_);
}

void MouseButtonCallback_C(GLFWwindow* window, int button, int action,
                           int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  if (action == GLFW_PRESS) {
    objects->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->BindDefaultCallbacks();
    objects->ui_cloning_.Stop(glfw_context->cursor_pos_tex_norm_);
    objects->ui_cloning_.ShowConfigWindow();
  }
}

}  // namespace objects
