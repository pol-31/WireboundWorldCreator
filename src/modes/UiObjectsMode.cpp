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
                             UiEditSlots& ui_edit_slots,
                             UiEditConfigSlTxt& value_config,
                             ModelManager& mdl_manager)
    : IUiMode(ui_shared_resources, {data::VboIdMain::kObjectsObjectsMode}),
      ui_selection_(ui_shared_resources),
      mouse_transform_(ui_shared_resources),
      ui_slots_(ui_slots),
      ui_edit_(ui_shared_resources, ui_edit_slots, value_config, mdl_manager),
      map_points_(mdl_manager),
      mdl_manager_(mdl_manager) {}

void UiObjectsMode::Setup() {
  ui_shared_resources_.glfw_context_.text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kHp),
      static_cast<int>(data::TextId::kAttackSpeed) + 1);
  UpdateModelsList();
  ui_slots_.Setup(&ui_edit_, [this] {
    std::vector<MapPoint>* points = nullptr;
    std::vector<glm::uvec2>* joints = nullptr;
    std::vector<glm::quat>* rotates = nullptr;
    std::vector<glm::vec3>* scales = nullptr;
    if (ui_slots_.GetSelectedSlotId() != -1) {
      points = &ui_edit_.GetInstanceData().map_points;
      joints = &ui_edit_.GetInstanceData().map_joints;
      rotates = &ui_edit_.GetInstanceData().rotations;
      scales = &ui_edit_.GetInstanceData().scales;
    }
    map_points_.SetData(points, joints, rotates, scales);
    map_points_.UpdateJointsBuffer();
  });
  BindDefaultCallbacks();
  auto camera = ui_shared_resources_.glfw_context_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f);  // to update camera vectors
  ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
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

void UiObjectsMode::Render() {
  ui_shared_resources_.glfw_context_.tile_renderer->Render();
  ui_selection_.Render();
  if (ui_slots_.GetSelectedSlotId() != -1) {
    auto color = ui_slots_.GetInstanceBaseData()->color;
    map_points_.RenderPoints(color);
    auto map_scale =
        ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_scale;
    map_points_.RenderJoints(ui_shared_resources_.glfw_context_.tile_renderer
                                 ->cur_tile_.map_terrain_height,
                             map_scale, color);
  }
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();
  sp_mode_.Render();

  auto mouse_pos = ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.glfw_context_.windows->Render();
  auto camera = ui_shared_resources_.glfw_context_.camera;
  camera->Update();  // const pos
}

void UiObjectsMode::RenderPicking() {
  ui_shared_resources_.glfw_context_.tile_renderer->RenderPicking();
  map_points_.RenderPickingPoints();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  ui_slots_.RenderPicking();
  ui_shared_resources_.glfw_context_.windows->RenderPicking();
}

void UiObjectsMode::UpdateModelsList() {
  ui_slots_.Reset();
  ui_edit_.SetModels(mdl_manager_.GetLoadedModels());
}

void UiObjectsMode::HandleSelection(const std::set<GLuint>& selected_ids) {
  auto map_points = map_points_.GetPoints();
  anything_selected_ = false;
  for (int i = 0; i < map_points->size(); ++i) {
    auto it = selected_ids.find(i);
    if (it != selected_ids.end()) {
      anything_selected_ = true;
      (*map_points)[i].selected = true;
    } else {
      (*map_points)[i].selected = false;
    }
  }
}

void UiObjectsMode::CancelTransform() {
  BindDefaultCallbacks();
  auto map_points = map_points_.GetPoints();
  auto rotates = map_points_.GetRotates();
  auto scales = map_points_.GetScales();
  mouse_transform_.TranslateSelectedMapPointsBack(*map_points);
  mouse_transform_.RotateSelectedMapPointsBack(*map_points, *rotates);
  mouse_transform_.ScaleSelectedMapPointsBack(*map_points, *scales);
  map_points_.UpdateJointsBuffer();
}

void UiObjectsMode::ApplyTransform() { BindDefaultCallbacks(); }

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
    objects->ui_slots_.Scroll(pressed_id, yoffset);
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
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        objects->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      objects->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_, mod_ctrl,
                                   mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (pressed_id > details::kIdOffsetObjects &&
          objects->anything_selected_ && pressed_id < details::kIdOffsetUi) {
        objects->map_points_.AddJoints(pressed_id);
      } else if (pressed_id < details::kIdOffsetWater) {
        return objects->map_points_.AddPoint(pressed_id);
      }
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
  objects->mouse_transform_.InitTransform();
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
  objects->mouse_transform_.SetAxis(key, mods & GLFW_MOD_SHIFT);
}

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  auto map_points = objects->map_points_.GetPoints();
  objects->mouse_transform_.TranslateSelectedMapPoints(*map_points, xpos, ypos);
  objects->map_points_.UpdateJointsBuffer();
}

void CursorPosCallback_R(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  auto map_points = objects->map_points_.GetPoints();
  auto rotates = objects->map_points_.GetRotates();
  objects->mouse_transform_.RotateSelectedMapPoints(*map_points, *rotates, xpos,
                                                    ypos);
}

void CursorPosCallback_S(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  auto map_points = objects->map_points_.GetPoints();
  auto scales = objects->map_points_.GetScales();
  objects->mouse_transform_.ScaleSelectedMapPoints(*map_points, *scales, xpos,
                                                   ypos);
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
    if (objects->map_points_.GetPoints()) {
      auto selected_ids = objects->ui_selection_.StopIntoSet(
          glfw_context->cursor_pos_tex_norm_);
      objects->HandleSelection(selected_ids);
    }
  }
}

}  // namespace objects
